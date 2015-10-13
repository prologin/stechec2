
# Stechec project is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# The complete GNU General Public Licence Notice can be found as the
# `NOTICE' file in the root directory.
#
# Copyright (C) 2006 Prologin
#

require "gen/file_generator"

def camel_case(str)
  strs = str.split("_")
  strs.each { |s| s.capitalize! }
  strs.join
end

def cs_proto(fn)
  # Returns the prototype of a C function
  # WARNING: arrays are hard to handle in C...
  if fn.ret.is_array?
    rettype = "void"
  else
    rettype = fn.ret.name
  end

  if not fn.ret.is_simple?
    rettype = camel_case(rettype)
  end

  buf = "\t\tpublic " + rettype + " " + camel_case(fn.name) + "("

  # Handle arguments
  args = []
  fn.args.each do |arg|
    if not arg.type.is_array?
      args = args << "#{arg.type} #{arg.name}"
    else
      args = args << "#{arg.type.type.name}[] #{arg.name}_arr"
    end
  end
  if fn.ret.is_array?
    args = args << "#{fn.ret.type.name}[] ret_arr"
  end
  if args.empty?
    args = args << ""
  end
  buf += args.join(", ")
  buf + ")\n"
end

# C++ generator, for C# interface
class CSharpCxxFileGenerator < CxxProto
  def initialize
    super
    @lang = "C++ (for C# interface)"
  end

  def generate_header()
    @f = File.open(@path + @header_file, 'w')
    print_banner "generator_cs.rb"

    @f.puts <<-EOF
#ifndef INTERFACE_HH_
# define INTERFACE_HH_

#include <map>
#include <vector>
#include <string>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/threads.h>

typedef int32_t gint32;

class CSharpInterface
{
public:
  CSharpInterface();
  ~CSharpInterface();
  MonoObject*   callCSharpMethod(const char* name);
  MonoImage*    getImage();
  MonoDomain*   getDomain();

private:

  MonoDomain*   _domain;
  MonoAssembly* _assembly;
  MonoImage*    _image;
  MonoClass*    _class;
  MonoObject*   _object;
};

    EOF

    build_enums
    build_structs

    @f.puts "", 'extern "C" {', ""
    for_each_fun { |fn| @f.print cxx_proto(fn, "api_"); @f.puts ";" }
    for_each_user_fun { |fn| @f.print cxx_proto(fn); @f.puts ";" }
    @f.puts "}", "", "#endif // !INTERFACE_HH_"
    @f.close
  end

  def build_common_wrappers
    @f.puts <<-EOF
template < class Out, class Cxx >
Out cxx2lang(Cxx in)
{
  return (Out)in;
}

template <>
MonoString* cxx2lang<MonoString*, std::string>(std::string in)
{
  return mono_string_new (gl_csharp.getDomain(), in.c_str());
}

template <>
gint32 cxx2lang< gint32, int >(int in)
{
  return (gint32)in;
}

template <>
gint32 cxx2lang< gint32, bool >(bool in)
{
  return (gint32)in;
}

template < class Out, class Cxx >
Cxx lang2cxx(Out in)
{
  return (Cxx)in;
}

template <>
std::string lang2cxx< MonoString*, std::string >(MonoString* in)
{
  std::string s_out;
  MonoError error;
  char *c_out;

  if (!in)
    return std::string("(null)");
  c_out = mono_string_to_utf8_checked(in, &error);
  if (!mono_error_ok(&error)) {
    s_out = std::string(mono_error_get_message(&error));
    mono_error_cleanup(&error);
    return s_out;
  } else {
    s_out = std::string(c_out);
    mono_free(c_out);
    return s_out;
  }
}

template <>
int lang2cxx< gint32, int >(gint32 in)
{
  return (int)in;
}

template <>
bool lang2cxx< gint32, bool >(gint32 in)
{
  return (bool)in;
}

template <>
MonoArray* cxx2lang< MonoArray*, std::vector<int> >(std::vector<int> in)
{
  gint32 size = in.size();
  MonoClass* mcKlass = mono_get_int32_class();
  MonoArray * maArray = mono_array_new(gl_csharp.getDomain(), mcKlass, size);

  for (int i = 0; i < size; ++i)
    mono_array_set(maArray, gint32, i, (cxx2lang< gint32, int >(in[i])));

  return maArray;
}

template <>
std::vector<int> lang2cxx< MonoArray*, std::vector<int> >(MonoArray* in)
{
  std::vector< int > out;
  gint32 size = mono_array_length(in);

  for (int i = 0; i < size; ++i)
    out.push_back(lang2cxx< gint32, int >(mono_array_get(in, gint32, i)));

  return out;
}
EOF
  end

  def build_enum_wrappers(enum)
    name = enum['enum_name']
    @f.puts <<-EOF
template <>
gint32 cxx2lang< gint32, #{name} >(#{name} in)
{
  return (gint32)in;
}

template <>
#{name} lang2cxx< gint32, #{name} >(gint32 in)
{
  return (#{name})in;
}
EOF
  end

  def build_array(typex, tn)
    if tn == "enum"
      name = typex['enum_name']
    elsif tn == "struct"
      name = typex['str_name']
    else
      name = typex
    end
    @f.puts <<-EOF
template <>
MonoArray* cxx2lang< MonoArray*, std::vector<#{name}> >(std::vector<#{name}> in)
{
  gint32 size = in.size();
  MonoClass* mcKlass = mono_class_from_name(gl_csharp.getImage(), "Prologin", \"#{camel_case(name)}\");
  MonoArray * maArray = mono_array_new(gl_csharp.getDomain(), mcKlass, size);
  for (int i = 0; i < size; ++i)
    EOF
    if tn == "struct"
      @f.puts "    mono_array_setref(maArray, i, (cxx2lang< MonoObject*, #{name} >(in[i])));"
    else
      @f.puts "    mono_array_set(maArray, gint32, i, (cxx2lang< gint32, #{name} >(in[i])));"
    end
    @f.puts <<-EOF
  return maArray;
}

template <>
std::vector<#{name}> lang2cxx< MonoArray*, std::vector<#{name}> >(MonoArray* in)
{
  std::vector< #{name} > out;
  gint32 size = mono_array_length(in);

  for (int i = 0; i < size; ++i)
    EOF
    if tn == "struct"
      @f.puts "    out.push_back(lang2cxx< MonoObject*, #{name} >(reinterpret_cast<MonoObject*>(mono_array_get(in, MonoObject*, i))));"
    else
      @f.puts "    out.push_back(lang2cxx< gint32, #{name} >(mono_array_get(in , gint32, i)));"
    end
    @f.puts <<-EOF
  return out;
}
    EOF
  end

  def cmonotype(ft)
    if ft.is_array?
      "MonoArray*"
    elsif ft.is_struct?
      "MonoObject*"
    elsif ft.is_nil?
      "void"
    elsif ft.name == "string"
      "MonoString*"
    else
      "gint32"
    end
  end

  def build_struct_wrappers(str)
    name = str['str_name']
    @f.puts <<-EOF
template <>
MonoObject* cxx2lang< MonoObject*, #{name} >(#{name} in)
{
  void* arg;
  MonoClass*  mcKlass  = mono_class_from_name(gl_csharp.getImage(), \"Prologin\", \"#{camel_case(name)}\");
  MonoObject* moObj    = mono_object_new(gl_csharp.getDomain(), mcKlass);
mono_runtime_object_init(moObj);
    EOF
    str['str_field'].each do |f|
      fn = f[0]
      ft = @types[f[1]]
      @f.puts <<-EOF
  arg = reinterpret_cast< void* >(cxx2lang< #{cmonotype(ft)}, #{cxx_type(ft)} >(in.#{fn}));
  mono_field_set_value(moObj, mono_class_get_field_from_name(mcKlass, \"#{camel_case(fn)}\"), &arg);
      EOF
    end
    @f.puts <<-EOF
  return moObj;
}

template <>
#{name} lang2cxx< MonoObject*, #{name} >(MonoObject* in)
{
    #{name} out;
  void*      field_out;
  MonoClass* mcKlass = mono_class_from_name(gl_csharp.getImage(), \"Prologin\", \"#{camel_case(name)}\");
  (void)field_out;
    EOF
    str['str_field'].each do |f|
      fn = f[0]
      ft = @types[f[1]]
      if ft.is_array? or ft.is_struct? then
        @f.puts <<-EOF
  mono_field_get_value(in, mono_class_get_field_from_name(mcKlass, \"#{camel_case(fn)}\"), &field_out);
  out.#{fn} = lang2cxx< #{cmonotype(ft)}, #{cxx_type(ft)} >(reinterpret_cast< #{cmonotype(ft)} >(field_out));
        EOF
      else
        @f.puts  "  mono_field_get_value(in, mono_class_get_field_from_name(mcKlass, \"#{camel_case(fn)}\"), &out.#{fn});"
      end
    end
    @f.puts "  return out;", "}", ""
  end

  def generate_source()
    @f = File.open(@path + @source_file, 'w')
    print_banner "generator_cs.rb"

    @f.puts <<-EOF
#include "interface.hh"

#include <iostream>
#include <assert.h>
#include <cstdlib>
#include <cstring>

CSharpInterface gl_csharp;

    EOF
    build_common_wrappers
    for_each_enum   { |e| build_enum_wrappers e; build_array(e,"enum") }

    for_each_struct { |s| build_struct_wrappers s; build_array(s,"struct") }

    build_helper_funcs
    target = $conf['conf']['player_lib']
    @f.puts <<-EOF

/*
** Inititialize Mono and load the DLL file.
*/
CSharpInterface::CSharpInterface()
{
  const char* champion_path = getenv("CHAMPION_PATH");
  std::string champion;

  if (!champion_path)
    champion = "./#{target}-prologin.dll";
  else
  {
    champion = champion_path;
    champion += "/#{target}-prologin.dll";
  }

  _domain = mono_jit_init(champion.c_str());
  assert(_domain != NULL);

  _assembly = mono_domain_assembly_open(_domain, champion.c_str());
  assert(_assembly != NULL);

  _image = mono_assembly_get_image(_assembly);
  assert(_image != NULL);

  _class = mono_class_from_name(_image, "Prologin", "Prologin");
  assert(_class != NULL);

  _object = mono_object_new(_domain, _class);
  assert(_object);

  mono_runtime_object_init(_object);

  // Register API functions as internal Mono functions
    EOF

    for_each_fun(false) do |fn|
      @f.puts "  mono_add_internal_call(\"Prologin.Api::" + camel_case(fn.name) + "\", (const void*)" + fn.name + ");"
    end

    @f.puts <<-EOF
}

MonoImage* CSharpInterface::getImage()
{
    return _image;
}

MonoDomain* CSharpInterface::getDomain()
{
    return _domain;
}

CSharpInterface::~CSharpInterface()
{
  mono_image_close(_image);
  mono_assembly_close(_assembly);
  // XXX -- mono segfaults when calling this. Seems to be a known bug
  //        appearing when mono_jit_clean() is called from a dtor. ???
  //mono_jit_cleanup(_domain);
}

/*
** Calls C# functions from C++
*/
MonoObject* CSharpInterface::callCSharpMethod(const char* name)
{
  MonoThread*   thread = mono_thread_attach(_domain);
  MonoMethod*   method = mono_class_get_method_from_name(_class, name, 0);
  MonoObject*   object = mono_runtime_invoke(method, _object, NULL, NULL);

  mono_thread_detach(thread);

  return object;
}

/*
** Functions called from stechec to C.
*/
    EOF

    for_each_user_fun(false) do |fn|
      @f.print cxx_proto(fn, '')
      if cxx_type(fn.ret) != "void"
        print_body "  return lang2cxx< #{cmonotype(fn.ret)}, #{cxx_type(fn.ret)} >(gl_csharp.callCSharpMethod(\"" + camel_case(fn.name) + "\"));"
      else
        print_body "  gl_csharp.callCSharpMethod(\"" + camel_case(fn.name) + "\");"
      end
    end
    @f.close
  end

  def build_helper_funcs
    for_each_fun(false) do |fn|
      @f.print cmonotype(fn.ret) + " " + fn.name + "("
      args = fn.args.map { |arg|
        at = cmonotype(arg.type)
        "#{at} #{arg.name}"
      }
      @f.puts args.join(", ") + ")"
      @f.puts "{"
      @f.print "\t"
      call = "api_" + fn.name + "("
      args = fn.args.map { |arg|
        "lang2cxx< " + cmonotype(arg.type) + ", #{cxx_type(arg.type)} >(#{arg.name})" }
        call += args.join(", ") + ")"
        if not fn.ret.is_nil?
          call = "return cxx2lang< #{cmonotype(fn.ret)}, #{cxx_type(fn.ret)} >(" + call + ")"
        end
        @f.puts call, ";", "}", ""
    end
  end

  def build
    @path = Pathname.new($install_path) + "cs"
    @header_file = 'interface.hh'
    @source_file = 'interface.cc'

    generate_header
    generate_source
  end

end

class CSharpFileGenerator < CSharpProto
  def initialize
    super
    @lang = "cs"
  end

  def print_comment(str)
    @f.puts '// ' + str if str
  end

  def print_multiline_comment(str, prestr = '')
    return unless str
    str.each_line {|s| @f.print prestr + '// ', s }
    @f.puts ""
  end

  def generate_api()
    @f = File.open(@path + @api_file, 'w')
    print_banner "generator_cs.rb"

    @f.puts "using System.Runtime.CompilerServices;", ""

    @f.puts "namespace Prologin {"

    build_enums
    build_structs

    @f.puts "\tclass Api {"
    build_constants
    for_each_fun do |fn|
      @f.puts "\t\t[MethodImplAttribute(MethodImplOptions.InternalCall)]"
      print_proto(fn, "\t\tpublic static extern");
      @f.puts ";"
    end
    @f.puts "\t}", "}"

    @f.close
  end

  def generate_makefile
    target = $conf['conf']['player_lib']
    @f = File.open(@path + "Makefile", 'w')
    @f.print <<-EOF
# -*- Makefile -*-

lib_TARGETS = #{target}

# Tu peux rajouter des fichiers source ici
#{target}-srcs = #{@source_file}

# Evite de toucher a ce qui suit
#{target}-dists = interface.hh
#{target}-srcs += api.cs interface.cc
#{target}-cxxflags = -fPIC \
    $(shell pkg-config --cflags glib-2.0 mono-2)
#{target}-ldflags = -lm \
    $(shell pkg-config --libs glib-2.0 mono-2)
#{target}-csflags = -target:library -nowarn:0169,0649

STECHEC_LANG=cs
include ../includes/rules.mk
EOF
@f.close
  end


  def build()
    @path = Pathname.new($install_path) + "cs"
    @source_file = $conf['conf']['player_filename'] + '.cs'
    @api_file = 'api.cs'

    ######################################
    ##  interface.hh file generation    ##
    ######################################
    CSharpCxxFileGenerator.new.build

    generate_api

    ######################################
    ##  prologin.cs file generation     ##
    ######################################
    @f = File.new(@path + @source_file, 'w')
    print_banner "generator_cs.rb"
    # Required stuff to call C from C#
    @f.puts  "using System;", ""

    @f.puts "namespace Prologin {", "", "\tclass Prologin {", ""

    for_each_user_fun(false) do |fn|
      @f.print cs_proto(fn)
      @f.puts "\t\t{", "\t\t\t// Place ton code ici", "\t\t}"
    end

    @f.puts "\t}", "}"
    @f.close

    generate_makefile
  end
end
