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

def g_c_type(type)
  if type.name == "bool" then
    "int"
  elsif type.name == "string" then
    "charp"
  else
    type.name
  end
end

def c_type(type)
  g_c_type(type)
end

def c_proto(fn, array_like_c = true)
  # Returns the prototype of a C function
  # WARNING: arrays are hard to handle in C...
  buf = ""
  if fn.ret.is_array?
    if array_like_c then
      rettype = "void"
    elsif fn.ret.name == "string" then "char*"
    else
      rettype = "#{c_type(fn.ret.type)}*"
    end
  else
    rettype = c_type(fn.ret)
  end

  buf += rettype + " " + fn.name + "("

  # Handle arguments
  args = []
  fn.args.each do |arg|
    if not arg.type.is_array?
      type = c_type(arg.type)
      args = args << "#{type} #{arg.name}"
    else
      if array_like_c then
        args = args << "#{arg.type.type.name}* #{arg.name}_arr"
        args = args << "size_t #{arg.name}_len"
      else
        args = args << "#{arg.type.type.name}* #{arg.name}"
      end
    end
  end
  if fn.ret.is_array? and array_like_c then
    args = args << "#{fn.ret.type.name}** ret_arr"
    args = args << "size_t* ret_len"
  end
  if args.empty?
    args = args << "void"
  end
  buf += args.join(", ")
  buf + ")"
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
  return in.error;
}

template<>
std_string lang2cxx<charp, std_string>(charp in)
{
  return in;
}

template<>
int lang2cxx<int, int>(int in)
{
  return in;
}

template<>
bool lang2cxx<int, bool>(int in)
{
  return in;
}

template<typename Lang, typename Cxx>
void lang2cxx_array(Lang* tab, size_t len, std::vector<Cxx>& vect)
{
  vect.reserve(len);
  for (size_t i = 0; i < len; ++i)
    vect.push_back( lang2cxx<Lang, Cxx>(tab[i]) );
}

template<typename Lang, typename Cxx>
Lang cxx2lang(Cxx in)
{
  return in.error;
}

template<>
charp cxx2lang<charp, std_string>(std_string in)
{
  size_t l = in.length();
  char * out = (char *) malloc(l + 1);
  for (int i = 0; i < l; i++) out[i] = in[i];
  out[l] = 0;
  return out;
}

template<>
int cxx2lang<int, bool>(bool in)
{
  return in;
}

template<>
int cxx2lang<int, int>(int in)
{
  return in;
}

template<typename Lang, typename Cxx>
void cxx2lang_array(Lang** tab, size_t* len, const std::vector<Cxx>& vect)
{
  *len = vect.size();
  *tab = (Lang *)malloc((*len) * sizeof(Lang));
  for (int i = 0; i < *len; ++i)
    (*tab)[i] = cxx2lang<Lang, Cxx>(vect[i]);
}
EOF
    for_each_enum do |x|
      name = x['enum_name']
      @f.puts "template<>"
      @f.puts "#{name} lang2cxx<#{name}, #{name}>(#{name} in) {"
      @f.puts " return in;"
      @f.puts "}"
      @f.puts "template<>"
      @f.puts "#{name} cxx2lang<#{name}, #{name}>(#{name} in) {"
      @f.puts " return in;"
      @f.puts "}"
    end

    for_each_struct do |x|
      def aux(fields, name_fun, lang, cxx, ref)
        fields['str_field'].each do |f|
          name =f[0]
          type = @types[f[1]]
          if type.is_array? then type_ = type.type
          else type_ = type
          end
          lang_type = c_type(type_)
          cxx_type = cxx_type(type_)
          if type.is_array? then
            @f.puts "#{name_fun}_array<#{lang_type}, #{cxx_type}>(#{ref}#{lang}.#{name}_arr, #{ref}#{lang}.#{name}_len, #{cxx}.#{name});"
          else
            @f.puts "out.#{name} = #{name_fun}<#{lang_type}, #{cxx_type}>(in.#{name});"
          end
        end
      end
      c_name = x['str_name']
      cxx_name = "__internal__cxx__#{c_name}"
      @f.puts "template<>"
      @f.puts "#{cxx_name} lang2cxx<#{c_name}, #{cxx_name}>(#{c_name} in) {"
      @f.puts "#{cxx_name} out;"
      aux(x, "lang2cxx", "in", "out", "")
      @f.puts " return out;"
      @f.puts "}"
      @f.puts "template<>"
      @f.puts "#{c_name} cxx2lang<#{c_name}, #{cxx_name}>(#{cxx_name} in) {"
      @f.puts "#{c_name} out;"
      aux(x, "cxx2lang", "out", "in", "&")
      @f.puts " return out;"
      @f.puts "}"
    end
    
    @f.puts <<-EOF
extern "C" {
EOF
    for_each_fun do |fn|
      @f.puts c_proto(fn)
      @f.puts "{"
      if not fn.ret.is_nil? then
        @f.print "  ", cxx_type(fn.ret), " ", "_retval;\n"
      end
      # Check if some of the arguments need conversion
      fn.args.each do |arg|
        if arg.type.is_array? then
          # Make a vector from the arr + len
          arrname = arg.name + "_arr"
          lenname = arg.name + "_len"
          name = arg.name
          lang_type = c_type(arg.type.type)
          cxx_type = cxx_type(arg.type.type)
          @f.puts "  std::vector<#{cxx_type}> param_#{ name };"
          @f.puts "  lang2cxx_array<#{lang_type}, #{cxx_type}>(#{arrname}, #{lenname}, param_#{name});"
        else
          name = arg.name
          lang_type = c_type(arg.type)
          cxx_type = cxx_type(arg.type)
          @f.puts "  #{cxx_type} param_#{ name } = lang2cxx<#{lang_type}, #{cxx_type}>(#{name});"
        end  
      end
      if not fn.ret.is_nil? then
        @f.print "  _retval = "
      end
      @f.print "api_", fn.name, "("
      argnames = fn.args.map { |arg| "param_#{arg.name}" }
      @f.print argnames.join(", ")
      @f.puts ");"

      unless fn.ret.is_nil?
        if not fn.ret.is_array? then
          @f.print "  return "
          ln = c_type(fn.ret)
          cxx = cxx_type(fn.ret)
          @f.puts "cxx2lang<#{ln}, #{cxx}>(_retval);"
        else
          @f.print "  "
          ln = c_type(fn.ret.type)
          cxx = cxx_type(fn.ret.type)
          @f.puts "cxx2lang_array<#{ln}, #{cxx}>(ret_arr, ret_len, _retval);"
        end
      end

      @f.puts "}"
    end
    @f.puts "}"
    @f.close
  end

  def cxx_type(type)
    cxx_type_for_pascal_and_c(type)
  end

  def generate_header
    @f = File.open(@path + @header_file, 'w')
    print_banner "generator_c.rb"
    @f.puts "#ifndef INTERFACE_HH_", "# define INTERFACE_HH_"
    @f.puts "", "# include <vector>", "# include <string>", ""
    @f.puts 'extern "C" {'
    @f.puts "# include \"#{$conf['conf']['player_filename']}.h\""
    @f.puts "}", ""
    @f.puts "typedef std::string std_string;"
    build_struct_for_pascal_and_c_to_cxx
    for_each_fun do |fn|
      @f.print cxx_proto(fn, "api_", 'extern "C"'), ";\n"
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
    @f.puts "#include <stdlib.h>"
    @f.puts "typedef char * charp;"
    build_constants
    build_enums
    build_structs_generic do |field, type|
      type = @types[type]
      if type.is_array? then
        "  size_t #{field}_len;\n  #{type.type.name} *#{field}_arr;"
      else "  #{c_type(type)} #{field};"
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
#{target}-cflags = -ggdb3 -Wall -Wextra

# Evite de toucher a ce qui suit
#{target}-dists += #{@header_file} interface.hh
#{target}-srcs += #{@source_file} ../includes/main.cc
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

