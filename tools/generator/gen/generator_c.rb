# -*- ruby -*-
# Stechec project is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# The complete GNU General Public Licence Notice can be found as the
# `NOTICE' file in the root directory.
#
# Copyright (C) 2005, 2006, 2007 Prologin
#

require "pathname"
require "gen/file_generator"

def get_array_types()
  def add_array_types(store, type)
    if type.is_array?
      add_array_types(store, type.type)
      store = store << [c_typename(type), type.type]
    end
  end

  array_types = []
  for_each_struct(false) do |x|
    x['str_field'].each do |f|
      add_array_types(array_types, @types[f[1]])
    end
  end
  for_each_fun(false) do |fn|
    add_array_types(array_types, fn.ret)
    fn.args.each do |arg|
      add_array_types(array_types, arg.type)
    end
  end
  for_each_user_fun(false) do |f|
    add_array_types(array_types, f.ret)
    f.args.each do |arg|
      add_array_types(array_types, arg.type)
    end
  end
  array_types.uniq
end

def cxx_typename(type)
  if type.is_array? then
    "std::vector<#{cxx_typename(type.type)}>"
  elsif type.is_struct?
    "__internal__cxx__#{type.name}"
  elsif type.is_simple? && type.name == "string"
    "std::string"
  else
    type.name
  end
end

def c_typename(type)
  if type.name == "string"
    "char*"
  elsif type.is_array?
    c_typename(type.type) + "_array"
  else
    type.name
  end
end

def c_proto(fn)
  # Returns the prototype of a C function
  args = fn.args.map { |arg| "#{c_typename(arg.type)} #{arg.name}" }.join(", ")
  "#{c_typename(fn.ret)} #{fn.name}(#{(args.empty? ? "void" : args)})"
end

def cxx_internal_proto(fn)
  # Returns the prototype of a CXX function
  args = fn.args.map { |arg| "#{cxx_typename(arg.type)} #{arg.name}" }.join(", ")
  "extern \"C\" #{cxx_typename(fn.ret)} api_#{fn.name}(#{(args.empty? ? "void" : args)})"
end

def get_lang2cxx(type)
  ctype = c_typename(type)
  cxxtype = cxx_typename(type)
  if type.is_array? and type.name != "string"
    # We want to remove the "_array" suffix to get the underlying type
    # We also want to remove the "std::vector<...>"
    "lang2cxx_array<#{ctype[0..-7]}, #{ctype}, #{cxxtype[12..-2]} >"
  else
    "lang2cxx<#{ctype}, #{cxxtype} >"
  end
end

def get_cxx2lang(type)
  ctype = c_typename(type)
  cxxtype = cxx_typename(type)
  if type.is_array? and type.name != "string"
    # We want to remove the "_array" suffix to get the underlying type
    # We also want to remove the "std::vector<...>"
    "cxx2lang_array<#{ctype[0..-7]}, #{ctype}, #{cxxtype[12..-2]} >"
  else
    "cxx2lang<#{ctype}, #{cxxtype} >"
  end

end

class CCxxFileGenerator < CxxProto
  def initialize
    super
    @lang = "C++ (for C interface)"
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
      ctype = x['str_name']
      cxxtype = "__internal__cxx__#{ctype}"
      @f.puts <<-EOF
template<>
#{cxxtype} lang2cxx<#{ctype}, #{cxxtype}>(#{ctype} in)
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
#{ctype} cxx2lang<#{ctype}, #{cxxtype}>(#{cxxtype} in)
{
  #{ctype} out;
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

  def generate_header
    @f = File.open(@path + @header_file, 'w')
    print_banner "generator_c.rb"
    @f.puts "#ifndef INTERFACE_HH_", "# define INTERFACE_HH_"
    @f.puts "", "# include <vector>", "# include <string>", ""
    @f.puts 'extern "C" {'
    @f.puts "# include \"#{$conf['conf']['player_filename']}.h\""
    @f.puts "}", ""

    for_each_struct do |s|
      name = s['str_name']
      @f.puts "typedef struct __internal__cxx__#{name} {"
      s['str_field'].each do |f|
        @f.puts "  #{cxx_typename(@types[f[1]])} #{f[0]};"
      end
      @f.puts "} __internal__cxx__#{name};"
    end

    for_each_fun do |fn|
      @f.print cxx_internal_proto(fn), ";\n"
    end
    @f.puts "#endif"
    @f.close
  end

  def build
    @path = Pathname.new($install_path) + "c"
    @source_file = 'interface.cc'
    @header_file = 'interface.hh'

    generate_source
    generate_header
  end
end

class CFileGenerator < CProto
  def generate_header
    @f = File.open(@path + @header_file, 'w')
    print_banner "generator_c.rb"
    @f.puts "#pragma once", ""
    @f.puts "#include <stdlib.h>", "#include <stdbool.h>", ""
    build_constants
    build_enums

    array_types = get_array_types()

    # Forward declaration in case of interdependencies
    for_each_struct(false) { |s| @f.puts "struct #{s['str_name']};" }
    @f.puts ""

    # Declaration of the array structures.
    array_types.each do |name, type|
      @f.puts "typedef struct #{name} {"
      if type.is_struct?
        # [0..-7] removes the "_array"
        @f.puts "  struct #{name[0..-7]}* datas;"
      else
        @f.puts "  #{name[0..-7]}* datas;"
      end
      @f.puts "  size_t length;", "} #{name};", ""
    end

    build_structs_generic do |field, type|
      type = @types[type]
      if type.is_array? and not type.name == "string"
        "  struct #{c_typename(type)} #{field};"
      else
        "  #{c_typename(type)} #{field};"
      end
    end

    for_each_fun do |f|
      @f.print c_proto(f), ";\n"
    end
    for_each_user_fun do |f|
      @f.print c_proto(f), ";\n"
    end
    @f.close
  end

  def generate_source()
    @f = File.open(@path + @user_file, 'w')
    print_banner "generator_c.rb"
    print_include @header_file
    @f.puts
    for_each_user_fun do |f|
      @f.print c_proto(f)
      print_body "  /* fonction a completer */"
    end
    @f.close
  end

  def generate_makefile
    target = $conf['conf']['player_lib']
    @f = File.open(@path + "Makefile", 'w')
    @f.print <<-EOF
# -*- Makefile -*-

lib_TARGETS = #{target}

# Tu peux rajouter des fichiers sources, headers, ou changer
# des flags de compilation.
#{target}-srcs = #{@user_file}
#{target}-dists =
#{target}-cflags = -ggdb3 -Wall -Wextra -std=c11

# Evite de toucher a ce qui suit
#{target}-dists += #{@header_file} interface.hh
#{target}-srcs += #{@source_file}
STECHEC_LANG=c
include ../includes/rules.mk
    EOF
    @f.close
  end


  def build()
    CCxxFileGenerator.new.build

    @path = Pathname.new($install_path) + "c"
    @header_file = $conf['conf']['player_filename'] + '.h'
    @source_file = 'interface.cc'
    @user_file = $conf['conf']['player_filename'] + '.c'

    generate_header
    generate_source
    generate_makefile
  end
end

