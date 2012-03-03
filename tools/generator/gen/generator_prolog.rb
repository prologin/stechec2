# -*- ruby -*-
# Stechec project is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# The complete GNU General Public Licence Notice can be found as the
# `NOTICE' file in the root directory.
#
# Copyright (C) 2005, 2006 Prologin
#

require "pathname"

# C generator with some Caml specificity
class PrologCFileGenerator < CxxProto

  def initialize
    super
    @lang = "C++ with Prolog extension"
  end

  def generate_header
    @f = File.open(@path + @header_file, 'w')
    print_banner "generator_prolog.rb"
    print_include "SWI-Prolog.h", true
    print_include "cstring", true 
    print_include "vector", true

    build_enums
    build_structs

    for_each_fun do |fn|
      @f.print cxx_proto(fn, "api_", 'extern "C"'); @f.puts ";"
    end
    for_each_user_fun do |fn|
      @f.print cxx_proto(fn, "", 'extern "C"'); @f.puts ";"
    end
    @f.close
  end

  def proto(fn)
    buf = "extern \"C\" foreign_t pl_#{fn.name}(term_t t)"
  end

  def build_common_wrappers
    @f.puts <<EOF

#define MODULE "user"

using namespace std;

void error(const char* expected, term_t got){
  std:cout << "error... expected " << expected << "\\n";
  PL_halt(1);
  exit(1);
}
void check(int rval) {
  if (!rval){
    std::cout << "error\\n";
    PL_halt(1);
    exit(1);
  }
}

template <typename Cxx>
void cxx2lang(Cxx in, term_t out)
{
}

template <typename Cxx>
void lang2cxx(term_t in, Cxx &out)
{
}

template <>
void lang2cxx<int>(term_t in, int &out)
{
  if (PL_is_integer(in)) {
    PL_get_integer(in, &out);
  }else{
    error("integer", in);
  }
}

template <>
void cxx2lang<int>(int in, term_t out)
{
  PL_put_integer(out, in);
}

template <typename Cxx>
void cxx2lang_array(std::vector<Cxx> in, term_t out)
{
  int size = in.size();
  PL_put_nil(out);
  for (int i = size - 1; i >= 0 ; i --)
  {
    term_t item = PL_new_term_ref();
    cxx2lang<Cxx>(in[i], item);
    PL_cons_list(out, item, out);
  }
}

// file:///usr/share/doc/swi-prolog-doc/Manual/foreigninclude.html
template <typename Cxx>
void lang2cxx_array(term_t in, std::vector<Cxx> &out)
{
  term_t head = PL_new_term_ref();      /* variable for the elements */
  term_t list = PL_copy_term_ref(in);    /* copy as we need to write */
  while( PL_get_list(list, head, list) )
  {
    Cxx t;
    lang2cxx<Cxx>(head, t);
    out.pushback(t);
  }
}
EOF
  end

  def build_enum_wrappers(enum)
    cxx_type = enum['enum_name']
    size = enum['enum_field'].length()
    @f.print "

template <>
void lang2cxx<", cxx_type,">(term_t in, "+cxx_type+" &out)
{
  atom_t a;
  PL_get_atom(in, &a);
  const char *s = PL_atom_chars(a);
"
    i = 0;
    enum['enum_field'].each do |f|
      @f.print "  if (strcmp(\"", f[0],"\", s) == 0) out =(", cxx_type,") ",i,";\n"
      i = i + 1;
    end
@f.print "}

template <>
void cxx2lang<", cxx_type,">(",cxx_type," in, term_t out)
{
  switch(in)
  {
"
    enum['enum_field'].each do |f|
      @f.print "    case ", f[0].upcase," :  PL_unify_atom_chars(out, \"",f[0],"\"); break;\n"
    end
    @f.print "  }\n}\n"
  end

  def build_struct_wrappers(struct)
    cxx_type = struct['str_name']
    size = struct['str_field'].length()
    args = struct['str_field'].map do |f| f[0] end
    args = args.join ', '

    @f.print "

template <>
void lang2cxx<", cxx_type,">(term_t in, "+cxx_type+" &out)
{
  functor_t f = PL_new_functor(PL_new_atom(\"",cxx_type,"\"), ",size,");
  term_t t = PL_new_term_ref();
  PL_unify_functor(t, f);\n"
    i = 0;
    struct['str_field'].each do |f|
      @f.print "  lang2cxx<",f[1],">(t + ", i, ", out.",f[0],");"
      i = i + 1
    end
@f.print "}

template <>
void cxx2lang<", cxx_type,">(",cxx_type," in, term_t out)
{
  functor_t f = PL_new_functor(PL_new_atom(\"",cxx_type,"\"), ",size,");
"
    struct['str_field'].each do |f|
      @f.print "  term_t ", f[0]," = PL_new_term_ref();\n"
      @f.print "  cxx2lang<",f[1],">(in.", f[0], ", ", f[0], ");\n"
    end
    @f.print "  PL_cons_functor(out, f, ",args,");\n"
    @f.print "}\n\n"
  end

  def generate_source
    @f = File.open(@path + @source_file, 'w')
    print_banner "generator_prolog.rb"

    print_include "string.h", true
    print_include "SWI-Prolog.h", true
    print_include "vector", true
    print_include "iostream", true

    print_include @header_file
    @f.puts

    build_common_wrappers
    for_each_enum { |enum| build_enum_wrappers enum }
    for_each_struct { |struct| build_struct_wrappers struct }

    for_each_fun do |fn|
      @f.puts proto(fn)
      @f.puts "{"
      i = 0
      fn.args.each do |arg|
        if arg.type.is_array? then
          suffixe = "_array"
          type = arg.type.type.name
          f_type = "std::vector<#{type}>"
        else
          suffixe = ""
          type = arg.type.name
          f_type = type
        end
        i = i + 1
        @f.print "  ", f_type, " arg_", i, ";\n"
        @f.print "  lang2cxx#{suffixe}<",type,">(t + ", i, ", arg_",i,");\n";
      end
      if fn.ret.is_array? then
        suffixe = "_array"
        type = fn.ret.type.name
        f_type = "std::vector<#{type}>"
      else
        suffixe = ""
        type = fn.ret.name
        f_type = type
      end
      i = 0
      args = fn.args.map do |f| i = i + 1; "arg_#{i}" end
      args = args.join ', '
      if fn.ret.is_nil? then
        @f.print "  api_", fn.name, "(", args, ");\n"
      else
        @f.print "  ", f_type, " out = api_", fn.name, "(", args, ");\n"
        @f.print "  cxx2lang#{suffixe}<",type,">(out, t);\n"
      end
        @f.puts "}\n"
    end

    for_each_user_fun do |fn|
      if fn.ret.is_nil? then
        n = 1
      else
        n = 0
      end
      n += fn.args.length();
      n = [1, n].max
      @f.puts cxx_proto(fn, "", 'extern "C"');
      @f.puts '{'
      @f.puts '  predicate_t pred = PL_predicate("'+fn.name+'", 1, MODULE);'
      @f.print '  term_t args = PL_new_term_refs(', n, ");\n" # todo arguments
      @f.puts '  check(PL_call_predicate(NULL, PL_Q_NORMAL, pred, args));'
      if fn.ret.is_nil? then
        @f.puts "  return;"
      else
        @f.puts cxx_type(fn.ret) + " out;"
        @f.puts ' lang2cxx<' + cxx_type(fn.ret) + '>(args, out);';
        @f.puts "return out;"
      end
      @f.puts '}'
    end

    @f.close
  end

  def build
    @path = Pathname.new($install_path) + "prolog"
    @header_file = 'interface.hh'
    @source_file = 'interface.cc'
    generate_header
    generate_source
  end

end


class PrologFileGenerator < FileGenerator
  def initialize
    super
    @lang = "prolog"
  end

  def print_comment(str)
    @f.puts '% ' + str
  end

  def print_multiline_comment(str)
    return unless str
    str.each {|s| @f.print "% ", s }
    @f.puts ""
  end

  def print_constant(type, name, val)
      @f.print 'const(', val,', ', name.downcase, ").\n"
  end
  
  def generate_makefile
    target = $conf['conf']['player_lib']
    @f = File.open(@path + "Makefile", 'w')
    # TODO
    @f.print <<-EOF
#
# Makefile
#

SRC       = prologin.pl # Ajoutez ici vos fichiers .pl
NAME      = #{$conf['conf']['player_lib']}.so
MY_CFLAGS =
AUXFILES  = interface.cc interface.hh

include ../includes/makeprolog
    EOF
    @f.close
  end

  def build
    @path = Pathname.new($install_path) + "prolog"
    @file = $conf['conf']['player_filename'] + '.pl'

    # Build C interface files
    PrologCFileGenerator.new.build

    @f = File.open(@path + @file, 'w')
    print_banner "generator_prolog.rb"

    build_constants
    # function to be completed
    @f.puts
    for_each_user_fun do |fn|
      @f.print fn.name, "(void) :-  % Pose ton code ici\n."
    end
    @f.close
    generate_makefile
  end
end
