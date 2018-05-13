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
require "gen/generator_c"

def rust_typename(type)
  if type.name == "string"
    "*const char"
  elsif type.name == "bool"
    "bool"
  elsif type.is_array?
     "Array<" + rust_typename(type.type) + ">"
  elsif ["int", "float", "double"].include? type.name
    "c_" + type.name
  else
    camel_case(type.name)
  end
end

def rust_proto(fn)
  # Returns the prototype of a C function
  args = fn.args.map { |arg| "#{arg.name}: #{rust_typename(arg.type)}" }.join(", ")
  ret = fn.ret.is_nil? ? "" : " -> #{rust_typename(fn.ret)}"
  "fn #{fn.name}(#{args})" + ret
end

class CCxxFileGeneratorForRust < CCxxFileGenerator
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

  def build_structs_generic(&show_field)
    for_each_struct do |x|
      @f.puts "#[repr(C)]"
      @f.puts "#[derive(Clone)]"
      @f.puts "pub struct #{camel_case(x['str_name'])} {"
      x['str_field'].each do |f|
        @f.print "  /// ", f[2], "\n"
        @f.print "  pub ", show_field.call(f[0], f[1]), ", \n"
      end
      @f.print "}\n\n"
    end
  end

  def generate_api
    @f = File.open(@path + @api_file, 'w')
    print_banner "generator_rust.rb"
        @f.puts <<-EOF
#![allow(unused)]
use std::os::raw::{c_double, c_int, c_void};

#[repr(C)]
#[derive(Clone)]
pub struct Array<T> {
    pub ptr: *const T,
    pub len: usize,
}

impl<T> Array<T> {
    // Doit etre appele exactement une fois par `Array`
    // retourne par une fonction de l'API.
    pub unsafe fn free(self) {
      free(self.ptr as _);
    }
}
EOF

    build_constants
    build_enums

    build_structs_generic do |field, type|
      type = @types[type]
      "#{field}: #{rust_typename(type)}"
    end

    @f.puts "extern {"
    @f.puts "fn free(p: *mut c_void);", ""
    for_each_fun do |f|
      @f.print "pub ", rust_proto(f), ";\n"
    end
    @f.puts "}", ""
    for_each_user_fun do |f|
      @f.puts"#[no_mangle]"
      @f.print "pub unsafe extern \"C\" ", rust_proto(f)
      args = f.args.map { |arg| arg.name }.join(", ")
      print_body "  super::#{f.name}(#{args})"
    end
    @f.close
  end

  def generate_user
    @f = File.open(@path + @user_file, 'w')
    @f.puts "#![allow(unused)]"
    @f.puts "pub mod api;"
    @f.puts "use api::*;"
    @f.puts "use std::os::raw::{c_double, c_int, c_void};", ""
    for_each_user_fun do |f|
      @f.print rust_proto(f)
      print_body "  // fonction a completer"
    end
    @f.close
  end

  def generate_makefile
    target = $conf['conf']['player_lib']
    @f = File.open(@path + "Makefile", 'w')
    @f.print <<-EOF
# -*- Makefile -*-

lib_TARGETS = #{target}

# Tu peux rajouter des fichiers sources ou changer des flags de compilation.
#{target}-dists =
#{target}-rustcflags = -g

# Evite de toucher a ce qui suit
#{target}-dists += #{$conf['conf']['player_filename']}.h interface.hh #{@api_file}
#{target}-srcs += interface.cc #{@user_file}
#{target}-ldflags = -fPIC -Wl,--whole-archive
#{target}-ldlibs  = -Wl,--no-whole-archive -lm -lrt -ldl -lpthread -lstdc++

STECHEC_LANG=rust
include ../includes/rules.mk
    EOF
    @f.close
  end


  def build()
    CCxxFileGeneratorForRust.new.build

    @path = Pathname.new($install_path) + "rust"
    @user_file = $conf['conf']['player_filename'] + '.rs'
    @api_file = 'api.rs'

    header_generator = CFileGenerator.new
    header_generator.path = Pathname.new($install_path) + "rust"
    header_generator.header_file = $conf['conf']['player_filename'] + '.h'

    header_generator.generate_header
    generate_user
    generate_api
    generate_makefile
  end
end
