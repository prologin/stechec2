# -*- ruby -*-
# Stechec project is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# The complete GNU General Public Licence Notice can be found as the
# `NOTICE' file in the root directory.
#
# Copyright (C) 2005, 2006, 2007, 2018 Prologin
#

require "pathname"
require "gen/file_generator"

def rust_ffi_typename(type)
  if type.name == "string"
    "*const char"
  elsif type.name == "bool"
    "bool"
  elsif type.is_array?
     "Array<" + rust_ffi_typename(type.type) + ">"
  elsif ["int", "float", "double"].include? type.name
    "c_" + type.name
  else
    camel_case(type.name)
  end
end

def rust_api_typename(type)
  if type.is_array?
     "Vec<" + rust_api_typename(type.type) + ">"
   else
     rust_ffi_typename(type)
   end
end

def rust_ffi_proto(fn)
  # Returns the prototype of a C function
  args = fn.args.map { |arg| "#{arg.name}: #{rust_ffi_typename(arg.type)}" }.join(", ")
  ret = fn.ret.is_nil? ? "" : " -> #{rust_ffi_typename(fn.ret)}"
  "fn #{fn.name}(#{args})" + ret
end

def rust_api_proto(fn)
  # Returns the prototype of a C function
  args = fn.args.map { |arg| "#{arg.name}: &#{rust_api_typename(arg.type)}" }.join(", ")
  ret = fn.ret.is_nil? ? "" : " -> #{rust_api_typename(fn.ret)}"
  "fn #{fn.name}(#{args})" + ret
end

class RustCxxFileGenerator < CCxxFileGenerator
  def initialize
    super
    @lang = "C++ (for Rust interface)"
  end

  def generate_source
    @f = File.open(@path + @source_file, 'w')
    print_banner "generator_c.rb"
    @f.puts <<-EOF
#include "interface.hh"
#include <cstdlib>

template<typename Lang, typename Cxx>
Cxx lang2cxx(Lang in)
{
  return in;
}

template<>
std::string lang2cxx<char*, std::string>(char* in)
{
  return in;
}

template<typename Lang, typename Lang_array, typename Cxx>
std::vector<Cxx> lang2cxx_array(Lang_array in)
{
  std::vector<Cxx> out(in.length);
  for (size_t i = 0; i < in.length; ++i)
    out[i] = lang2cxx<Lang, Cxx>(in.datas[i]);
  free(in.datas);
  return out;
}

template<typename Lang, typename Cxx>
Lang cxx2lang(Cxx in)
{
  return in;
}

template<>
char* cxx2lang<char*, std::string>(std::string in)
{
  size_t l = in.length();
  char* out = (char *) malloc(l + 1);
  for (int i = 0; i < l; i++) out[i] = in[i];
  out[l] = 0;
  return out;
}

template<typename Lang, typename Lang_array, typename Cxx>
Lang_array cxx2lang_array(const std::vector<Cxx>& in)
{
  Lang_array out = { NULL, in.size() };
  out.datas = (Lang *)malloc((out.length) * sizeof(Lang));
  for (int i = 0; i < out.length; ++i)
    out.datas[i] = cxx2lang<Lang, Cxx>(in[i]);
  return out;
}
EOF

    for_each_struct do |x|
      rtype = x['str_name']
      cxxtype = "__internal__cxx__#{rtype}"
      @f.puts <<-EOF
template<>
#{cxxtype} lang2cxx<#{rtype}, #{cxxtype}>(#{rtype} in)
{
  #{cxxtype} out;
EOF
      x['str_field'].each do |f|
        name = f[0]
        type = @types[f[1]]
        @f.puts "  out.#{name} = #{get_lang2cxx(type)}(in.#{name});"
      end
      @f.puts <<-EOF
  return out;
}

template<>
#{rtype} cxx2lang<#{rtype}, #{cxxtype}>(#{cxxtype} in)
{
  #{rtype} out;
EOF
      x['str_field'].each do |f|
        name = f[0]
        type = @types[f[1]]
        @f.puts "  out.#{name} = #{get_cxx2lang(type)}(in.#{name});"
      end
      @f.puts "  return out;", "}", ""
    end

    @f.puts <<-EOF
extern "C" {
EOF
    for_each_fun do |fn|
      @f.puts c_proto(fn)
      buffer = "api_#{fn.name}("
      buffer += fn.args.map { |arg| "#{get_lang2cxx(arg.type)}(#{arg.name})" }.join(", ") + ")"
      buffer = "return #{get_cxx2lang(fn.ret)}(#{buffer})" if not fn.ret.is_nil?
      @f.puts "{", "  #{buffer};", "}", ""
    end
    @f.puts "}"
    @f.close
  end

  def build
    @path = Pathname.new($install_path) + "rust"
    @source_file = 'interface.cc'
    @header_file = 'interface.hh'

    generate_source
    generate_header
  end
end

class RustFileGenerator < FileGenerator
  def print_multiline_comment(str, prestr = '')
    str.each_line {|s| @f.print prestr, "// ", s }
    @f.puts
  end

  # must be called right after print_proto
  def print_body(content)
    @f.puts "", "{"
    @f.puts content
    @f.puts "}"
  end

  def print_constant(type, name, val)
      @f.puts "pub const #{name}: c_#{type} = #{val};"
  end

  def build_enums
    for_each_enum do |x|
      @f.puts "#[repr(C)]"
      @f.puts "#[derive(Clone, Copy, Debug, Eq, Hash, PartialEq, PartialOrd, Ord)]"
      @f.puts "pub enum #{camel_case(x['enum_name'])} {"
      x['enum_field'].each do |f|
        name = camel_case(f[0])
        @f.print "  /// ", f[1], "\n"
        @f.print "  ", name, ", \n"
      end
      @f.puts "}"
    end
  end

  def build_struct_generic(struct, &typename)
    name = camel_case(struct['str_name'])
    @f.puts "pub struct #{camel_case(struct['str_name'])} {"
    struct['str_field'].each do |f|
      type = @types[f[1]]
      type = typename.call(type)
      @f.puts "  /// #{f[2]}"
      @f.puts "  pub #{f[0]}: #{type},"
    end
    @f.puts "}"
    @f.puts
  end

  def impl_conv_id(type)
    @f.puts "impl ToAPI for #{type} {"
    @f.puts "  type To = #{type};"
    @f.puts "  unsafe fn to_api(&self) -> Self::To { self.clone() }"
    @f.puts "}"
    @f.puts
    @f.puts "impl ToFFI for #{type} {"
    @f.puts "  type To = #{type};"
    @f.puts "  fn to_ffi(&self) -> Self::To { self.clone() }"
    @f.puts "}"
    @f.puts
  end

  def generate_ffi
    @f = File.open(@path + @ffi_file, 'w')
    print_banner "generator_rust.rb"
    @f.puts "use super::api::{self,"
    for_each_enum do |x|
      @f.print "#{camel_case(x['enum_name'])},"
    end
    @f.puts "};"
        @f.puts <<-EOF
use std::{mem, ptr, slice};
use std::os::raw::{c_float ,c_double, c_int, c_void};

#[repr(C)]
#[derive(Clone)]
pub struct Array<T> {
    pub ptr: *const T,
    pub len: usize,
}

pub trait ToAPI {
  type To;
  unsafe fn to_api(&self) -> Self::To;
}

pub trait ToFFI {
  type To;
  fn to_ffi(&self) -> Self::To;
}

impl<T: ToAPI + Clone> ToAPI for Array<T> {
  type To = Vec<T::To>;
  unsafe fn to_api(&self) -> Self::To {
    let vec;
    {
      let slice = slice::from_raw_parts(self.ptr, self.len);
      vec = slice.iter().map(|elem| elem.to_api()).collect();
    }
    free(self.ptr as _);
    vec
  }
}

impl<T: ToFFI> ToFFI for Vec<T> {
  type To = Array<T::To>;
  fn to_ffi(&self) -> Self::To {
    unsafe {
      let ptr: *mut T::To = malloc(self.len() * mem::size_of::<T::To>()) as _;
      let len = self.len();
      for i in 0..len {
        ptr::write(ptr.add(i), self[i].to_ffi());
      }
      Array { ptr, len }
    }
  }
}

EOF

    for type in ["()", "c_int", "c_float", "c_double", "bool", "*const char"]
      impl_conv_id(type)
    end

    for_each_enum do |x|
      impl_conv_id(camel_case(x['enum_name']))
    end

    for_each_struct do |x|
      name = camel_case(x['str_name'])
      @f.puts "#[repr(C)]"
      @f.puts "#[derive(Clone)]"
      build_struct_generic(x) do |type|
        rust_ffi_typename(type)
      end

      @f.puts "impl ToAPI for #{name} {"
      @f.puts "  type To = api::#{name};"
      @f.puts "  unsafe fn to_api(&self) -> Self::To {"
      @f.puts "    api::#{name} {"
      x['str_field'].each do |f|
        @f.puts "      #{f[0]}: self.#{f[0]}.to_api(),"
      end
      @f.puts "    }"
      @f.puts "  }"
      @f.puts "}"
      @f.puts
      @f.puts "impl ToFFI for api::#{name} {"
      @f.puts "  type To = #{name};"
      @f.puts "  fn to_ffi(&self) -> Self::To {"
      @f.puts "    #{name} {"
      x['str_field'].each do |f|
        @f.puts "      #{f[0]}: self.#{f[0]}.to_ffi(),"
      end
      @f.puts "    }"
      @f.puts "  }"
      @f.puts "}"
      @f.puts
    end

    @f.puts "extern {"
    @f.puts "fn malloc(size: usize) -> *mut c_void;", ""
    @f.puts "fn free(p: *mut c_void);", ""
    for_each_fun do |f|
      @f.print "pub ", rust_ffi_proto(f), ";\n"
    end
    @f.puts "}"
    @f.close
  end

def generate_api
    @f = File.open(@path + @api_file, 'w')
    @f.puts <<-EOF
#![allow(unused)]
use super::ffi::{self, ToAPI, ToFFI};
use std::os::raw::{c_float, c_double, c_int, c_void};

EOF

    build_constants
    build_enums

    for_each_struct do |x|
      @f.puts "#[derive(Clone, Debug, PartialEq, PartialOrd)]"
      build_struct_generic(x) do |type|
        rust_api_typename(type)
      end
    end

    for_each_fun do |f|
      @f.print "pub ", rust_api_proto(f)
      args = f.args.map { |arg| "#{arg.name}.to_ffi()" }.join(", ")
      print_body "  unsafe { ffi::#{f.name}(#{args}).to_api() }"
    end

    for_each_user_fun do |f|
      @f.puts"#[no_mangle]"
      @f.print "pub unsafe extern \"C\" ", rust_ffi_proto(f)
      args = f.args.map { |arg| "&#{arg.name}.to_api()" }.join(", ")
      print_body "  super::#{f.name}(#{args}).to_ffi()"
    end
    @f.close
  end

  def generate_user
    @f = File.open(@path + @user_file, 'w')
    @f.puts "#![allow(unused)]"
    @f.puts "pub mod api;"
    @f.puts "mod ffi;"
    @f.puts
    @f.puts "use api::*;"
    @f.puts "use std::os::raw::{c_double, c_int, c_void};", ""
    for_each_user_fun do |f|
      @f.print rust_api_proto(f)
      print_body "  // fonction a completer"
    end
    @f.close
  end

  def generate_makefile
    target = $conf['conf']['player_lib']
    header_file = $conf['conf']['player_filename'] + ".h"
    @f = File.open(@path + "Makefile", 'w')
    @f.print <<-EOF
# -*- Makefile -*-

lib_TARGETS = #{target}

# Tu peux rajouter des fichiers sources ou changer des flags de compilation.
#{target}-dists =
#{target}-rustcflags = -g

# Evite de toucher a ce qui suit
#{target}-dists += #{header_file} interface.hh #{@ffi_file} #{@api_file}
#{target}-srcs += interface.cc #{@user_file}
#{target}-ldflags = -fPIC -Wl,--whole-archive
#{target}-ldlibs  = -Wl,--no-whole-archive -lm -lrt -ldl -lpthread -lstdc++

STECHEC_LANG=rust
include ../includes/rules.mk
    EOF
    @f.close
  end


  def build()
    RustCxxFileGenerator.new.build

    @path = Pathname.new($install_path) + "rust"
    @user_file = $conf['conf']['player_filename'] + '.rs'
    @ffi_file = 'ffi.rs'
    @api_file = 'api.rs'

    header_generator = CFileGenerator.new
    header_generator.path = Pathname.new($install_path) + "rust"
    header_generator.header_file = $conf['conf']['player_filename'] + '.h'

    header_generator.generate_header
    generate_user
    generate_ffi
    generate_api
    generate_makefile
  end
end
