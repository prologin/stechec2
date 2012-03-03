#
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

# C++ generator, for python-interface
class PythonCxxFileGenerator < CxxProto
  def initialize
    super
    @lang = "C++ (for python interface)"
  end


  def generate_header()
    @f = File.open(@path + @header_file, 'w')
    print_banner "generator_python.rb"

    @f.puts <<-EOF
#ifndef INTERFACE_HH_
# define INTERFACE_HH_

# include <Python.h>
# include <vector>
# include <string>

    EOF

    build_enums
    build_structs

    @f.puts "", 'extern "C" {', ""
    for_each_fun do |fn|
        @f.print cxx_proto(fn, "api_")
        @f.puts ";"
    end
    @f.puts "}", "", "#endif // !INTERFACE_HH_"
    @f.close
  end

  def build_common_wrappers
    @f.puts <<-EOF
template <typename Lang, typename Cxx>
Lang cxx2lang(Cxx in)
{
  return in.__if_that_triggers_an_error_there_is_a_problem;
}

template <>
PyObject* cxx2lang<PyObject*, int>(int in)
{
  return PyInt_FromLong(in);
}


template <>
PyObject* cxx2lang<PyObject*, std::string>(std::string in)
{
return PyString_FromString (in.c_str());
}


template <>
PyObject* cxx2lang<PyObject*, bool>(bool in)
{
  return PyBool_FromLong(in);
}

template <typename Cxx>
PyObject* cxx2lang_array(const std::vector<Cxx>& in)
{
  size_t size = in.size();
  PyObject* out = PyList_New(size);

  for (unsigned int i = 0; i < size; ++i)
    PyList_SET_ITEM(out, i, (cxx2lang<PyObject*, Cxx>(in[i])));

  return out;
}

template <typename Lang, typename Cxx>
Cxx lang2cxx(Lang in)
{
  return in.__if_that_triggers_an_error_there_is_a_problem;
}

template <>
int lang2cxx<PyObject*, int>(PyObject* in)
{
  long out = PyInt_AsLong(in);
  if (out == -1)
    if (PyErr_Occurred())
    {
      throw 42;
    }

  return out;
}

template <>
bool lang2cxx<PyObject*, bool>(PyObject* in)
{
  return (bool)lang2cxx<PyObject*, int>(in);
}

template <>
std::string lang2cxx<PyObject*, std::string>(PyObject* in)
{
  char * out = PyString_AS_STRING(in);
  if (PyErr_Occurred())
    {
      throw 42;
    }
  return out;
}

template <typename Cxx>
std::vector<Cxx> lang2cxx_array(PyObject* in)
{
  if (!PyList_Check(in))
  {
    PyErr_SetString(PyExc_TypeError, "a list is required");
    throw 42;
  }

  std::vector<Cxx> out;
  unsigned int size = PyList_Size(in);

  for (unsigned int i = 0; i < size; ++i)
    out.push_back(lang2cxx<PyObject*, Cxx>(PyList_GET_ITEM(in, i)));

  return out;
}
EOF
  end

  def build_enum_wrappers(enum)
    name = enum['enum_name']
    @f.puts <<-EOF
template <>
PyObject* cxx2lang<PyObject*, #{name}>(#{name} in)
{
  return cxx2lang<PyObject*, int>((int)in);
}

template <>
#{name} lang2cxx<PyObject*, #{name}>(PyObject* in)
{
  return (#{name})lang2cxx<PyObject*, int>(in);
}
EOF
  end

  def build_struct_wrappers(str)
    name = str['str_name']
    nfields = str['str_field'].length
    @f.puts "template <>"
    @f.puts "PyObject* cxx2lang<PyObject*, #{name}>(#{name} in)"
    @f.puts "{"
    @f.puts "  PyObject* tuple = PyTuple_New(#{nfields});"
    i = 0
    str['str_field'].each do |f|
      fn = f[0]
      ft = @types[f[1]]
      @f.print "  PyTuple_SET_ITEM(tuple, #{i}, "
      if ft.is_array? then
        @f.print "cxx2lang_array(in.#{fn})"
      else
        @f.print "(cxx2lang<PyObject*, #{ft.name}>(in.#{fn}))"
      end
      @f.puts ");"
      i += 1
    end
    @f.puts "  PyObject* name = PyString_FromString(\"#{name}\");"
    @f.puts "  PyObject* cstr = PyObject_GetAttr(py_module, name);"
    @f.puts "  Py_DECREF(name);"
    @f.puts "  if (cstr == NULL) throw 42;"
    @f.puts "  PyObject* ret = PyObject_CallObject(cstr, tuple);"
    @f.puts "  Py_DECREF(cstr);"
    @f.puts "  Py_DECREF(tuple);"
    @f.puts "  if (ret == NULL) throw 42;"
    @f.puts "  return ret;"
    @f.puts "}"
    @f.puts

    # OTHER WAY
    @f.puts "template <>"
    @f.puts "#{name} lang2cxx<PyObject*, #{name}>(PyObject* in)"
    @f.puts "{"
    @f.puts "  #{name} out;"
    @f.puts "  PyObject* i;"
    i = 0
    str['str_field'].each do |f|
      fn = f[0]
      ft = @types[f[1]]
      @f.puts "  i = cxx2lang<PyObject*, int>(#{i});"
      @f.puts "  i = PyObject_GetItem(in, i);"
      @f.puts "  if (i == NULL) throw 42;"
      @f.print "  out.#{fn} = "
      if ft.is_array?
        @f.print "lang2cxx_array<#{ft.type.name}>("
      else
        @f.print "lang2cxx<PyObject*, #{ft.name}>("
      end
      @f.print "i);\n"
      @f.puts "  Py_DECREF(i);"
      i += 1
    end
    @f.puts "  return out;"
    @f.puts "}"
  end

  def build_function(fn)
    @f.puts "static PyObject* p_#{fn.name}(PyObject* self, PyObject* args)"
    @f.puts "{"
    @f.puts "  (void)self;"
    fs = "O" * fn.args.length
    i = 0
    fn.args.each do |a|
      @f.puts "PyObject* a#{i};"
      i += 1
    end
    @f.print "  if (!PyArg_ParseTuple(args, \"#{fs}\""
    @f.print ", " if fn.args.length != 0
    i = 0
    names = fn.args.map do |a|
      s = "&a#{i}"
      i += 1
      s
    end
    @f.print names.join(", ")
    @f.puts ")) {"
    @f.puts "    return NULL;"
    @f.puts "  }"
    @f.print "  "
    @f.puts "  try {"
    unless fn.ret.is_nil?
      @f.print "return "
      if fn.ret.is_array?
        @f.print "cxx2lang_array("
      else
        @f.print "cxx2lang<PyObject*, #{cxx_type(fn.ret)}>("
      end
    end
    @f.print "api_", fn.name, "("
    i = 0
    fn.args.each do |a|
      if a.type.is_array? then
        @f.print "lang2cxx_array<#{cxx_type(a.type.type)}>("
      else
        @f.print "lang2cxx<PyObject*, #{cxx_type(a.type)}>("
      end
      @f.print "a#{i})"
      i += 1
      @f.print ", " unless i == fn.args.length
    end
    @f.print ")" unless fn.ret.is_nil?
    @f.print ");\n"
    if fn.ret.is_nil?
      @f.puts "  Py_INCREF(Py_None);", "  return Py_None;"
    end
    @f.puts "  } catch (...) { return NULL; }"
    @f.puts "}"
  end

  def generate_source()
    @f = File.open(@path + @source_file, 'w')
    print_banner "generator_python.rb"
    
    @f.puts <<-EOF
#include "interface.hh"

static PyObject* c_module;
static PyObject* py_module;
static PyObject* champ_module;

static void _init_python();

    EOF
      
    build_common_wrappers
    for_each_enum { |e| build_enum_wrappers e }
    for_each_struct { |s| build_struct_wrappers s }
    for_each_fun { |fn| build_function fn }

    @f.puts <<-EOF

/*
** Api functions to register.
*/
static PyMethodDef api_callback[] = {
    EOF

    for_each_fun(false) do |fn|
      @f.print '  {"', fn.name, '", p_', fn.name, ', METH_VARARGS, "', fn.name, '"},'
    end

    @f.puts <<-EOF
  {NULL, NULL, 0, NULL}
};

static void _initapi()
{
  c_module = Py_InitModule("_api", api_callback);
}

/*
** Inititialize python, register API functions,
** and load .py file
*/
static void _init_python()
{
  PyObject* name;
  const char* champion_path;
      
  champion_path = getenv("CHAMPION_PATH");
  if (champion_path == NULL)
    champion_path = ".";

  setenv("PYTHONPATH", champion_path, 1);

  static char program_name[] = "stechec";
  Py_SetProgramName(program_name);
  Py_Initialize();
  _initapi();

  name = PyString_FromString("#{$conf['conf']['player_filename']}");
  champ_module = PyImport_Import(name);
  Py_DECREF(name);
  if (champ_module == NULL)
    if (PyErr_Occurred())
    {
      PyErr_Print();
      abort();
    }

  name = PyString_FromString("api");
  py_module = PyImport_Import(name);
  Py_DECREF(name);
  if (py_module == NULL)
    if (PyErr_Occurred())
    {
      PyErr_Print();
      abort();
    }
}

/*
** Run a python function.
*/
static PyObject* _call_python_function(const char* name)
{
  static bool initialized = false;

  if (!initialized)
  {
    initialized = true;
    _init_python();
  }

  PyObject *arglist, *func;
  PyObject *result = NULL;
  
  func = PyObject_GetAttrString(champ_module, (char*)name);
  if (func && PyCallable_Check(func))
  {
    arglist = Py_BuildValue("()");
    result = PyEval_CallObject(func, arglist);
    Py_XDECREF(arglist);
    Py_DECREF(func);
  }
  if (result == NULL && PyErr_Occurred())
    PyErr_Print();

  return result;
}

/*
** Functions called from stechec to C.
*/
    EOF

    for_each_user_fun(false) do |fn|
      @f.print cxx_proto(fn, '', 'extern "C"')
      @f.puts "", "{"
      @f.puts "  PyObject* _retval = _call_python_function(\"#{fn.name}\");"
      @f.puts "  if (!_retval && PyErr_Occurred()) { PyErr_Print(); abort(); }"
      @f.puts "  try {"
      if fn.ret.is_nil? then
        @f.puts "  Py_XDECREF(_retval);"
      elsif fn.ret.is_array? then
        @f.puts "  #{cxx_type(fn.ret)} ret = lang2cxx_array<#{cxx_type(fn.ret.type)}>(_retval);"
        @f.puts "  Py_XDECREF(_retval);"
        @f.puts "  return ret;"
      else
        @f.puts "  #{cxx_type(fn.ret)} ret = lang2cxx<PyObject*, #{cxx_type(fn.ret)}>(_retval);"
        @f.puts "  Py_XDECREF(_retval);"
        @f.puts "  return ret;"
      end
      @f.puts "  } catch (...) { PyErr_Print(); abort(); }"
      @f.puts "}",""
    end

    @f.close
  end

  def build
    @path = Pathname.new($install_path) + "python"
    @header_file = 'interface.hh'
    @source_file = 'interface.cc'

    generate_header
    generate_source
  end

end

class PythonFileGenerator < FileGenerator
  def initialize
    super
    @lang = "python"
  end

  def print_constant(type, name, val)
    @f.print name, " = ", val, "\n"
  end

  def print_comment(str)
    @f.puts '# ' + str if str
  end

  def print_multiline_comment(str)
    return unless str
    str.each_line {|s| @f.print '# ', s }
    @f.puts ""
  end

  def generate_makefile
    target = $conf['conf']['player_lib']
    @f = File.open(@path + "Makefile", 'w')
    @f.print <<-EOF
# -*- Makefile -*-

lib_TARGETS = #{target}

# Tu peux rajouter des fichiers source ici
#{target}-dists = #{@source_file}

# Evite de toucher a ce qui suit
#{target}-dists += api.py interface.hh
#{target}-srcs = interface.cc ../includes/main.cc

pc = $(shell which python2-config >/dev/null 2>&1 && echo python2-config || echo python-config)
#{target}-cxxflags = -fPIC $(shell $(pc) --includes)
#{target}-ldflags = -s $(shell $(pc) --ldflags)

include ../includes/rules.mk
    EOF
    @f.close
  end

  def build_enums
    for_each_enum do |enum|
      @f.puts "("
      enum['enum_field'].each do |f|
        name = f[0].upcase
        @f.print "    ", name, ", "
        @f.print "# <- ", f[1], "\n"
      end
      @f.print ") = range(", enum['enum_field'].length, ")\n\n"
    end
  end

  def build_structs
    @f.print <<-EOF
from collections import namedtuple

EOF
    for_each_struct do |x|
      @f.print x['str_name'], ' = namedtuple("', x['str_name'], '",', "\n"
      x['str_field'].each do |f|
        @f.puts "    '#{f[0]} ' # <- #{f[2]}"
      end
      @f.puts ")", ""
    end
  end

  def build()
    @path = Pathname.new($install_path) + "python"
    @source_file = $conf['conf']['player_filename'] + '.py'

    ######################################
    ##  interface.hh file generating    ##
    ######################################
    PythonCxxFileGenerator.new.build

    ######################################
    ##  prologin.py file generating     ##
    ######################################
    @f = File.new(@path + @source_file, 'w')
    @f.puts "# -*- coding: iso-8859-1 -*-"
    print_banner "generator_python.rb"
    @f.puts "from api import *", ""
    @f.puts "try:"
    @f.puts "    import psyco"
    @f.puts "    psyco.full()"
    @f.puts "except:"
    @f.puts "    pass"

    for_each_user_fun do |fn|
      @f.puts "def " + fn.name + "():"
      @f.puts "    pass # Place ton code ici"
    end
    @f.close

    @f = File.new(@path + "api.py", 'w')
    @f.puts "# -*- coding: iso-8859-1 -*-"
    @f.puts "from _api import *", ""
    build_constants
    build_enums
    build_structs
    @f.close
    generate_makefile
  end

end
