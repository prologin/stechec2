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

require "gen/file_generator"

# C++ generator, for java-interface
class RubyCxxFileGenerator < CxxProto

  def initialize
    super
    @lang = "C++ (for ruby interface)"
    @rb_types = TypesHash.new
    [ 'void', 'int', 'bool', 'VALUE' ].each do |x|
        @rb_types[x] = SimpleType.new "VALUE"
    end
  end

  def generate_header
    @f = File.open(@path + @header_file, 'w')
    print_banner "generator_ruby.rb"
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

  def generate_source
    #
    # Cxx interface file
    #
    @f = File.open(@path + @source_file, 'w')
    print_banner "generator_ruby.rb"
    print_include "ruby.h", true
    print_include "vector", true
    print_include "interface.hh"
    print_include "iostream", true
@f.puts "

/*
** execute 'str', protected from possible exception, ...
*/

void TYPEERR(char *t, VALUE in) {
  char err_str[256];
  sprintf(err_str, \"erreur de types : %s expected, got : 0x%02x.\\n\", t, TYPE(in));
  rb_fatal( err_str);
}

template<typename Cxx>
VALUE cxx2lang(Cxx in)
{
}

template<typename Cxx>
Cxx lang2cxx(VALUE in)
{
}

template<>
int lang2cxx<int>(VALUE in)
{
  if (TYPE(in) != T_FIXNUM) TYPEERR( (char*) \"integer\", in);
  return FIX2INT(in);
}

template<>
bool lang2cxx<bool>(VALUE in)
{
  return (in == T_TRUE)? true : false;
}

template<typename Cxx>
std::vector<Cxx> lang2cxx_array(VALUE l)
{
  if (TYPE(l) != T_ARRAY) TYPEERR((char*)\"array\", l);
  int len = RARRAY_LEN(l);
  VALUE *tab = RARRAY_PTR(l);
  std::vector<Cxx> vect;
  vect.reserve(len);
  for (int i = 0; i < len; i ++){
    vect.push_back( lang2cxx<Cxx>( tab[i] ) );
  }
  return vect;
}

template<>
VALUE cxx2lang<int>(int in)
{
  return INT2FIX(in);
}

template<>
VALUE cxx2lang<bool>(bool in)
{
  return in?T_TRUE:T_FALSE;
}

template<typename Cxx>
VALUE cxx2lang_array(const std::vector<Cxx>& vect)
{
  long len = vect.size();
  VALUE ary = rb_ary_new();
  for (int i = 0; i < len; ++i){
    rb_ary_push(ary, cxx2lang<Cxx>(vect[i]) );
  }
  return ary;
}
"

    for_each_enum do |e|
      ty = e['enum_name'];
      fields = e['enum_field']

      @f.puts "template<>"
      @f.puts "#{ty} lang2cxx<#{ty}>(VALUE in)\n{"
      @f.puts "if (TYPE(in) != T_STRING) TYPEERR( (char*) \"#{ty}\", in);"
      @f.puts "  char* v = RSTRING_PTR(in);"
      fields.each do |f|
        @f.puts "  if (strcmp(v, (char *) \"#{f[0]}\") == 0)"
        @f.puts "      return #{f[0].upcase};"
      end
      @f.puts "  abort();"
      @f.puts "  TYPEERR((char *) \"#{ty}\", in);"
      @f.puts "}\n"

      @f.puts "template<>"
      @f.puts "VALUE cxx2lang<#{ty}>(#{ty} in)\n{"
      @f.puts "  switch (in)\n  {"
      fields.each do |f|
        @f.puts "    case #{f[0].upcase}:"
        @f.puts "      return rb_str_new( (char *) \"#{f[0]}\", #{f[0].length});"
      end
      @f.puts "  }
  abort();
}\n"
    end


    for_each_struct do |s|
      type = s["str_name"]
      @f.puts "template<>"
      @f.puts "#{type} lang2cxx<#{type}>(VALUE in)\n{"
      @f.puts "  if (TYPE(in) != T_OBJECT) TYPEERR( (char *)\"#{type}\", in);"
      @f.puts "  #{type} out ;"
      s['str_field'].each do |f|
        name =f[0]
        type = @types[f[1]]
        if type.is_array? then
          type = type.type.name
          fun = '_array'
        else
          type = type.name
          fun = ''
        end
        @f.puts "  VALUE #{name} = rb_iv_get(in, (char *) \"@#{name}\");"
        @f.puts "  out.#{name} = lang2cxx#{fun}<#{type}>(#{name});"
      end
      @f.puts "  return out;"
      @f.puts "}\n"
      type = s["str_name"]
      @f.puts "template<>"
      @f.puts "VALUE cxx2lang<#{type}>(#{type} in)\n{"
      args = s['str_field'].map do |f|
        name =f[0]
        type0 = @types[f[1]]
        if type0.is_array? then
          type1 = type0.type.name
          fun = '_array'
        else
          type1 = type0.name
          fun = ''
        end
        o = "cxx2lang#{fun}<#{type1}>(in.#{name})"
      end
      @f.puts "  VALUE argv[] = {#{args.join ', '}};"
      @f.puts "  int argc = #{args.size };"
      @f.puts "  VALUE out = rb_funcall2(rb_path2class( (char *) \"#{type.capitalize()}\"), rb_intern( (char *) \"new\"), argc, argv);"
      @f.puts "  return out;"
      @f.puts "}"

    end
    # create callbacks
    for_each_fun(false) do |fn|
      name = fn.name
      type_ret = fn.ret
      args = fn.args
      args_str = (args.map do |arg| "VALUE #{arg.name}" end).join ', '
      if args == [] then virgule = "" else virgule = ", " end
      @f.print "static VALUE rb_#{name}(VALUE self#{virgule}#{args_str})"
      s_args = ""
      if args != []
        args_str = args.map do |y|
          if y.type.is_array? then
            arr = '_array'
          else
            arr = ''
          end
          fun_name = "lang2cxx#{arr}<#{y.type.name}>"
          fun_name+"( " + y.name + " ) "
        end
        s_args = args_str.join ', '
      end
      out = "api_#{name}(#{s_args})"
      if type_ret.is_nil? then
        print_body out+";\n  return Qnil;"
      else
        if type_ret.is_array? then
          arr = '_array'
        else
          arr = ''
        end
        ret_fun_name = "cxx2lang#{arr}<#{type_ret.name}>"
        print_body "  return #{ret_fun_name}(#{out});"
      end
    end
@f.puts "
void loadCallback()
{ 
"

    # configure callbacks in the Ruby environment
    for_each_fun do |x|
      args = x.args
      fct_name = x.name
      l = args ? args.length() : 0
      @f.puts "    rb_define_global_function( (char *) \"#{fct_name}\", (VALUE(*)(ANYARGS))(rb_#{fct_name}), #{l});"
    end

    @f.puts "
}

void init(){
  static bool initialized = false;
  if (!initialized){
    initialized = true;
    std::cout << \"init...\" << std::endl;
    std::string file;
    char * path = getenv(\"CHAMPION_PATH\");
    file = (path == NULL) ? \".\":path;
    std::cout << \"directory is \" << file << std::endl;
    setenv(\"RUBYLIB\", file.c_str(), 1);
    file += \"#{$conf['conf']['player_filename']}.rb\";
    int status;
    ruby_init();
    ruby_script(\"\");
    std::cout << \"load path...\" << std::endl;
    ruby_init_loadpath ();
    std::cout << \"load callback...\" << std::endl;
    loadCallback();
    std::cout << \"load file...\" << std::endl;
    rb_load_protect(rb_str_new2(file.c_str()), 0, &status);
    std::cout << \"status = \" << status << std::endl;
    if (status){
       rb_p (rb_errinfo()); 
       abort();
    }
  }
}

void my_ruby_end(){
  ruby_cleanup(0);
}

"
  
    @f.puts "extern \"C\" {"
    for_each_user_fun do |fn|
      name = fn.name
      ret = fn.ret
      args = fn.args

      fn.name = "#{fn.name}_unwrap";
      fn.ret = SimpleType.new("VALUE");
      fn.args = [FunctionArg.new(@rb_types, ["args", "VALUE"])];

      @f.print cxx_proto(fn)
      if ret.is_nil? then
        ret_str = ""
        decl_str = ""
      else
        if ret.is_array? then
          prefix="_array"
          ret = ret.type
        else
          prefix="";
        end
        ret_str = " return lang2cxx#{prefix}<#{cxx_type(ret)}>(v);"
        decl_str = "VALUE v ="
      end
      print_body "
  printf(\"calling ruby function: #{name}\\n\");
  VALUE v = rb_eval_string(\"#{name}()\"); 
  return v;
"
      fn.ret = ret
      fn.name = name;
      fn.args = args
      @f.print cxx_proto(fn)
      if fn.name == "end_game" then
        rb_end = "  my_ruby_end();\n"
      else
        rb_end = ""
      end
      print_body "
  int status;
  init();
  #{decl_str}rb_protect(&#{fn.name}_unwrap, Qnil, &status);
  if (status){
    fprintf(stderr, \"error while calling ruby function: #{name} (%d)\\n\", status);
    rb_p (rb_errinfo());
    #{rb_end}abort();
  }else {
    #{rb_end}#{ret_str}
  }"
    end
    @f.puts "}"
    @f.close
  end

  def build
    @path = Pathname.new($install_path) + "ruby"
    @source_file = 'interface.cc'
    @header_file = 'interface.hh'

    generate_header
    generate_source
  end

end

class RubyFileGenerator < CProto

  def print_comment(str)
    @f.puts '# ' + str if str
  end

  def print_multiline_comment(str)
    return unless str
    str.each_line {|s| @f.print '# ', s }
    @f.puts ""
  end

  def print_constant(type, name, val)
      @f.print name, ' = ', val, "\n"
  end

  def generate_source()

    #
    # Ruby contants file
    #
    @f = File.open(@path + @ruby_constant_file, 'w')
    print_banner "generator_ruby.rb"
    build_constants
    for_each_struct do |s|
      args = s['str_field'].map do |f| f[0] end
      @f.puts "class #{s['str_name'].capitalize()}"
      s['str_field'].each do |f|
        @f.puts "    attr_reader :#{f[0]} # #{f[1]}"
      end
      @f.puts "  def initialize(#{args.join ', '})"
      s['str_field'].each do |f|
        @f.puts "    @#{f[0]} = #{f[0]}"
      end
      @f.puts "  end"
      @f.puts "end"
    end


    @f.close

    #
    # Ruby main file
    #
    @f = File.open(@path + @source_file, 'w')
    print_banner "generator_ruby.rb"
    @f.puts "require '#{@ruby_constant_file}'"
    @f.puts
    for_each_user_fun(false) do |fn|
      name = fn.name
      args = fn.args.map do |a| a.name end
      @f.print "def ", name, "(#{args.join ', '})\n", "  # fonction a completer\n", "end\n" 
    end
    @f.close
  end

  def generate_makefile
    @f = File.open(@path + "Makefile", 'w')
    @f.print <<-EOF
#
# Makefile
#

NAME      = #{$conf['conf']['player_lib']}.so

include ../includes/makeruby
    EOF
    @f.close
  end


  def build()
    @path = Pathname.new($install_path) + "ruby"
    @ruby_constant_file = 'constants.rb'
    @source_file = $conf['conf']['player_filename'] + '.rb'

    RubyCxxFileGenerator.new.build
    generate_source
    generate_makefile
  end
end

