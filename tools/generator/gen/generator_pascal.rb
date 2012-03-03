#
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


module PascalUtils
  def conv_type(type, separray=' ')
    if type.is_simple? then
      {
        "bool" => "boolean",
        "int"  => "cint",
        "string" => "pchar",
      "void" => "void" # useless case
      }[type.name]
    elsif type.is_array? then
      "array#{separray}of#{separray}#{ conv_type type.type }"
    elsif type.name == "string"
      "char*"
    else
      type.name
    end
  end

  def print_comment(str)
    @f.puts '(* ' + str + ' *)' if str
  end

  def print_multiline_comment(str)
    return unless str
    @f.puts '(*'
    str.each_line {|s| @f.print "  ", s }
    @f.puts "\n*)"
  end

  def print_proto(fn)
    if fn.ret.is_nil?
      @f.print "procedure"
    else
      @f.print "function"
    end
    @f.print " ", fn.name
    args = fn.args.map do |arg|
      "#{arg.name} : #{ conv_type(arg.type, "_") }"
    end
    @f.print "(#{args.join("; ")})"
    if not fn.ret.is_nil?
      @f.print " : ", conv_type(fn.ret, "_")
    end
    @f.print ";"
  end

end

class PascalCxxFileGenerator < CxxFileGenerator
  def c_type(type)
    if type.name == "string" then "char*"
    elsif type.name == "bool" then "bool"
    else g_c_type(type)
    end
  end
  def cxx_type(type)
    cxx_type_for_pascal_and_c(type)
  end
  def lang_type(type)
    if type.is_array? then "#{lang_type(type.type)}*"
    elsif type.name == "string" then "char*"
    else type.name
    end
  end

  def initialize
    super
    @lang = "C++ (for C interface)"
  end

  def generate_source
    @f = File.open(@path + @source_file, 'w')
    print_banner "generator_pascal.rb"
    @f.puts <<-EOF
#include "interface.hh"
#include <cstdio>
#include <cstdlib>


extern "C"{
  void fpc_dynarray_setlength(void *, void *, int, void *);
  int fpc_dynarray_length(void *);
}


template<typename Lang, typename Cxx>
Cxx lang2cxx(Lang in)
{
  return in.error;
}

template<>
int lang2cxx<int, int>(int in)
{
  return in;
}
template<>
bool lang2cxx<bool, bool>(bool in)
{
  return in;
}

template<>
std::string lang2cxx<char*, std::string>(char * in)
{
  return in;
}

template<typename Lang, typename Cxx>
std::vector<Cxx> lang2cxx_array(Lang *l)
{
  std::vector<Cxx> vect;
  int len = fpc_dynarray_length(&l);
  vect.reserve(len);
  for (size_t i = 0; i < len; ++i)
    vect.push_back( lang2cxx<Lang, Cxx>(l[i]) );
  return vect;
}

template<typename Lang, typename Cxx>
Lang cxx2lang(Cxx in)
{
  return in.err;
}

template<>
char *cxx2lang<char*, std::string>(std::string in)
{
  size_t l = in.length();
  char * out = (char *) malloc(l + 1);
  for (int i = 0; i < l; i++) out[i] = in[i];
  out[l] = 0;
  return out;
}

template<>
int cxx2lang<int, int>(int in)
{
  return in;
}
template<>
bool cxx2lang<bool, bool>(bool in)
{
  return in;
}

template<typename Lang, typename Cxx>
Lang * cxx2lang_array(const std::vector<Cxx>& vect, void *type_ptr)
{
  long len = vect.size();
  Lang * tab = NULL;
  fpc_dynarray_setlength(&tab, type_ptr, 1, &len);
  for (int i = 0; i < len; ++i){
    tab[i] = cxx2lang<Lang, Cxx>(vect[i]);
  }
  return tab;
}
EOF

    @f.puts "extern int INIT_PROLO_INTERFACE_ARRAY_OF_CINT;";
    for_each_struct do |s|
      type = s["str_name"]
      @f.puts "extern int INIT_PROLO_INTERFACE_ARRAY_OF_#{type.upcase};"
    end
    for_each_enum do |s|
      type = s["enum_name"]
      @f.puts "extern int INIT_PROLO_INTERFACE_ARRAY_OF_#{type.upcase};"
    end
    
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
      def aux(fields, name_fun, lang, cxx, addon_ok)
        name_fun_initial = name_fun
        fields['str_field'].each do |f|
          name =f[0]
          type = @types[f[1]]
          if type.is_array? then type_ = type.type
          else type_ = type
          end
          lang_type = c_type(type_)
          cxx_type = cxx_type(type_)
          if type.is_array? then
            prefix=""
            if addon_ok then
              addon=", &INIT_PROLO_INTERFACE_ARRAY_OF_#{lang_type.upcase}"
            else
              addon = ""
            end
            name_fun = "#{name_fun}_array"
          else
            prefix=""
            addon=""
            name_fun = name_fun_initial
          end
          @f.puts "out.#{name} = #{name_fun}<#{lang_type}, #{cxx_type}>(#{prefix}in.#{name}#{addon});"
        end
      end
      c_name = x['str_name']
      cxx_name = "__internal__cxx__#{c_name}"
      @f.puts "template<>"
      @f.puts "#{cxx_name} lang2cxx<#{c_name}, #{cxx_name}>(#{c_name} in) {"
      @f.puts "#{cxx_name} out;"
      aux(x, "lang2cxx", "in", "out", false)
      @f.puts " return out;"
      @f.puts "}"
      @f.puts "template<>"
      @f.puts "#{c_name} cxx2lang<#{c_name}, #{cxx_name}>(#{cxx_name} in) {"
      @f.puts "#{c_name} out;"
      aux(x, "cxx2lang", "out", "in", true)
      @f.puts " return out;"
      @f.puts "}"
    end
    @f.puts "extern \"C\" {"
    for_each_fun do |fn|
      @f.puts c_proto(fn, false)
      @f.puts "{"
      if not fn.ret.is_nil?
      then @f.print "  ", cxx_type(fn.ret), " ", "_retval;\n"
      end

      # Check if some of the arguments need conversion
      fn.args.each do |arg|
        type = arg.type
        if type.is_array?
          cxx_type = cxx_type(type.type)
          lang_type = lang_type(type.type)
          if type.type.is_simple? && type.type.name == "int" then
            type_ptr = "CINT"
          else
            type_ptr = "#{lang_type.upcase}"
          end
          @f.puts "  #{cxx_type(arg.type)} arg_#{arg.name} = lang2cxx_array<#{lang_type}, #{cxx_type}>(#{arg.name}, &INIT_PROLO_INTERFACE_ARRAY_OF_#{type_ptr});"
        else
          cxx_type = cxx_type(type)
          lang_type = lang_type(type)
          @f.puts "  #{cxx_type(arg.type)} arg_#{arg.name} = lang2cxx<#{lang_type}, #{cxx_type}>(#{arg.name});"
        end
      end
      if not fn.ret.is_nil? # todo : factoriser ca.
      then
        @f.print "  _retval = "
        @f.print "api_", fn.name, "( "
        if fn.args != [] then
          @f.print "arg_"
        end
        argnames = fn.args.map { |arg| arg.name }
        @f.print argnames.join(", arg_")
        @f.puts " );"
        if fn.ret.is_array?
          suffix="_array"
          t = fn.ret.type
          ln = c_type(t)
          cxx = cxx_type(t)
          if fn.ret.type.is_simple? && fn.ret.type.name == "int" then
            suffix_ptr = "CINT"
          else
            suffix_ptr = ln.upcase
          end

          addon = ", &INIT_PROLO_INTERFACE_ARRAY_OF_#{suffix_ptr}"
        else
          addon = ""
          suffix=""
          t = fn.ret
          ln = c_type(t)
          cxx = cxx_type(t)
        end
        @f.puts "  return cxx2lang#{suffix}<#{ln}, #{cxx}>(_retval#{addon});"
      else
        @f.print "api_", fn.name, "( "
        if fn.args != [] then
          @f.print "arg_"
        end
        argnames = fn.args.map { |arg| arg.name }
        @f.print argnames.join(", arg_")
        @f.puts " );"
      end

      @f.puts "}"
    end
    @f.puts "}"
    @f.close
  end

  def generate_header
    @f = File.open(@path + @header_file, 'w')
    print_banner "generator_c.rb"
    @f.puts "#ifndef INTERFACE_HH_", "# define INTERFACE_HH_"
    @f.puts "", "# include <vector>", "# include <string>", ""
    build_constants
    build_enums
    build_structs_generic do |field, type|
      type = @types[type];
      if type.is_array?
        n = "#{type.type.name} *"
      else
        n = "#{type.name} "
      end
      "#{n}#{field};"
    end
    build_struct_for_pascal_and_c_to_cxx
    @f.puts 'extern "C" {'
    for_each_fun do |fn|
      @f.print cxx_proto(fn, "api_"), ";\n"
    end
    @f.puts '}'
    @f.puts "#endif"
    @f.close
  end

  def build
    @path = Pathname.new($install_path) + "pascal"
    @source_file = 'interface.cc'
    @header_file = 'interface.hh'

    generate_source
    generate_header
  end
end

class PascalFileGenerator < FileGenerator
  include PascalUtils

  def initialize
    super
    @lang = "C++ (for pascal interface)"
  end

  def build_enums
    for_each_enum do |x|
      name = x['enum_name']
      @f.puts "type #{name} =\n  (\n"
      fields = x['enum_field'].map do |f|
        name_field = f[0].downcase
        "    #{ name_field } { <- #{ f[1] } }"
      end
      @f.print "#{ fields.join(",\n") }\n  );\n\n"
      @f.print "type array_of_#{name} = array of #{name};\n\n"
    end
  end

  def build_structs
    for_each_struct do |x|
      name =x['str_name']
      @f.puts "type #{name} =\n  record\n"
      x['str_field'].each do |f|
        @f.print "    #{f[0]} : #{ conv_type(@types[f[1]]) }; "
        print_comment (" <- " + f[2])
      end
      @f.print "  end;\n\n"
      @f.print "type array_of_#{name} = array of #{name};\n\n"
    end
  end

  def print_constant(type, name, val)
    @f.print "\t", name
    (25 - name.to_s.length).times { @f.print " " }
    @f.print " =  ", val, ";"
  end

  def generate_makefile
    @f = File.open(@path + "Makefile", 'w')
    @f.print <<-EOF
#
# Makefile
#

SRC       = #{@source_file}.pas # Ajoutez ici vos fichiers .pas
NAME      = #{$conf['conf']['player_lib']}.so
MY_CFLAGS =
AUXFILES  = interface.cc interface.hh

include ../includes/makepascal
    EOF
    @f.close
  end

  def build()
    PascalCxxFileGenerator.new.build

    @path = Pathname.new($install_path) + "pascal"
    @source_file = $conf['conf']['player_filename']

    #
    # Generate c-interface file
    #
    @f = File.open(@path + ('prolo_interface.pas'), 'w')
    @f.puts "unit prolo_interface;", "interface", "uses CTypes;", ""
    print_banner "generator_pascal.rb"
    @f.puts "
type array_of_cint = array of cint;
type array_of_boolean = array of boolean;
"
    @f.puts "const\n"
    build_constants
    build_enums
    build_structs
    for_each_fun do |f|
      print_proto(f);
      @f.puts " cdecl; external;"
    end
    @f.puts "implementation", "", "begin", "end."
    @f.close

    #
    # Generate user file
    #
    @f = File.open(@path + (@source_file + '.pas'), 'w')
    @f.puts "library champion;","", "uses prolo_interface;", ""
    for_each_user_fun do |f|
      print_proto(f)
      @f.puts " cdecl; export;"
      @f.puts "begin", "\t(* fonction a completer *)", "end;"
    end
    @f.puts "", "exports"
    v = false
    for_each_user_fun false do |f|
      if v then @f.print "," end
      v = true
      @f.print "  #{ f.name }"
    end
    @f.print ";\n"
    @f.puts "", "begin", "end."
    @f.close

    #
    # Generate Makefile
    #
    generate_makefile
  end
end
