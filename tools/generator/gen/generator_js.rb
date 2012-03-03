# -*- ruby -*-
# Stechec project is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# The complete GNU General Public Licence Notice can be found as the
# `NOTICE' file in the root directory.
#
# Copyright (C) 2009 coucou747
#

require "pathname"

# C generator with some javascript specificity
class JsCFileGenerator < CxxProto

  def initialize
    super
    @lang = "C with js extension"
    @module_name = "prologin"
    @js_file = $conf["conf"]["player_filename"] + ".js"
  end

  def generate_header
    @f = File.open(@path + @header_file, 'w')
    print_banner "generator_js.rb"
    @f.close
  end

  def generate_source
    @f = File.open(@path + @source_file, 'w')
    print_banner "generator_js.rb"
    @f.puts "
#include <stdio.h>
#include <stdlib.h>
#include <mozjs/jsconfig.h>
#include <mozjs/jsapi.h>

#define MODULE_NAME \""+@module_name+"\"

JSRuntime *rt;
JSContext *cx;
JSObject  *global;
JSObject  *module;
JSScript *script;
char filename[1024];

/* The class of the global object. */
static JSClass global_class = {
    \"global\", JSCLASS_GLOBAL_FLAGS,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

/* The class of the module object. */
static JSClass module_class = {
    MODULE_NAME, JSCLASS_GLOBAL_FLAGS,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

/* The error reporter callback. */
void reportError(JSContext *cx, const char *message, JSErrorReport *report)
{
    fprintf(stderr, \"%s:%u:%s\\n\",
            report->filename ? report->filename : \"<no filename>\",
            (unsigned int) report->lineno,
            message);
}

void print_once(JSContext *cx, jsval j){
  if (JSVAL_IS_NULL(j)) {
    printf(\"null\");
  }else if (JSVAL_IS_VOID(j)){
    printf(\"void\");
  }else if (JSVAL_IS_BOOLEAN(j)){
    if (JSVAL_TO_BOOLEAN(j)){
      printf(\"true\");
    }else{
      printf(\"false\");
    }
  }else if (JSVAL_IS_INT(j)){
    printf(\"%d\", JSVAL_TO_INT(j));
  }else if (JSVAL_IS_DOUBLE(j)){
    printf(\"%lf\", *JSVAL_TO_DOUBLE(j));
  }else if (JSVAL_IS_STRING(j)){
    printf(\"%s\", JS_GetStringBytes(JS_ValueToString(cx, j)));
  }else{
    printf(\"Value cannot be displayed\");
  }
}

JSBool js_print(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
  int i;
  for (i=0; i<argc; i++){
    print_once(cx, argv[i]);
  }
  *rval = JSVAL_VOID;   /* return void*/
  return JS_TRUE; /* tout s'est bien deroule */
}

JSBool js_printl(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
  if (js_print(cx, obj, argc, argv, rval)){
    printf(\"\\n\");
    return JS_TRUE;
  }else{
    return JS_FALSE;
  }
}

";

    for_each_fun do |f|
      name = f.name
      type_ret = f.ret
      args = f.args
      if args != nil
	arrity = args.length;
      else
	arrity = 0
      end
      @f.print "
extern "+cxx_type(type_ret)+" "+name+"("
      if args != nil
	args.each_index do |i|
	    type = args[i].type;
	    arg = args[i].name;
	    if i != 0
	      @f.print ", ";
	    end
	  @f.print cxx_type(type)+" "+arg;
	end
      end
      @f.print ");
JSBool js_"+name+"(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
  #ifdef DEBUG
  printf(\"calling function "+name+"\");
  #endif
  if (argc != ",arrity,"){
    /* arite de la fonction */
    JS_ReportError(cx, \"bad args : arrity error, function must have ",arrity," args, %d given\", argc);
    return JS_FALSE;
  }\n"
      if type_ret.is_nil?
	@f.puts "*rval = JSVAL_VOID;   /* return void*/"
      else
	@f.print cxx_type(type_ret)+" out="+name+"(";
	if args != nil
          i = 0;
	  args_str = args.map do |arg|
	    type = arg.type;
	    arg = arg.name;
	    if type=="int"
	      str = "JSVAL_TO_INT"
	    else
	      if type=="double"
		str = "JSVAL_TO_DOUBLE"
	      else
                # TODO
                str = ""
	      end
	    end
	    str + "(argv[{i}])"
            i = i + 1
	  end
          @f.print args_str.join(", ")
	end
	@f.puts ");"
	if type_ret == "int"
	  @f.print "*rval = INT_TO_JSVAL(out);"
	else
	  print "erreur : type de retour : "+cxx_type(type_ret)+" non pris en charge\n"
	  print "fichier : "+__FILE__+"\nligne : #{__LINE__}\n"
	end
      end
      @f.puts "
      return JS_TRUE; /* tout s'est bien deroule */
    }"
    end
    @f.puts "

struct JSFunctionSpec myjs_global_functions[] = {
  {\"print\",   js_print, 0, 0, 0},
  {\"printl\",   js_printl, 0, 0, 0},
  {0}
};

struct JSFunctionSpec myjs_module_functions[] = {";
    for_each_fun do |f|
      name = f.name
      type_ret = f.ret
      args = f.args
      if args != nil
	arrity = args.length;
      else
	arrity = 0
      end
      @f.print "{\""+name+"\", js_"+name+", ",arrity,", 0, 0},\n"
    end
    @f.print " {0}
};
";
    for_each_user_fun do |f|
      name = f.name
      type_ret = f.ret
      args = f.args
      if args != nil
	arrity = args.length;
      else
	arrity = 0
      end
      if name == "init_game" or name == "end_game"
	jsname="re_"+name;
      else
	jsname=name;
      end
      if type_ret == nil
	type_ret = "void"
      end
      @f.print cxx_type(type_ret)+" "+jsname+"("
      @f.print "){
// calling user functions
jsval rval;
jsval argv[",arrity,"];\n"
      if args != nil
	args.each_index do |i|
	  type = args[i][1];
	  arg = args[i][0];
	  if type == "int"
	    @f.print "argv[",i,"] = INT_TO_JSVAL(",arg,");"
	  else
	    print "erreur : type de retour : "+type_ret+" non pris en charge\n"
	    print "fichier : "+__FILE__+"\n"
	    print "ligne : #{__LINE__}\n"
	  end
	end
      end
      @f.print "
  JSBool ok = JS_CallFunctionName(cx, global, \""+name+"\", ",arrity,", argv, &rval);
  if (ok){";
      if type_ret != "void"
	if type_ret == "int"
	  @f.print "
    if (JSVAL_IS_INT(rval)){
      return JSVAL_TO_INT(rval));
    }else{
      printf(\"erreur : return value is not an "+type_ret+"\\n\");
      return 0;
    }";
	else
	 print "type de retour non gere.\nfichier : #{__FILE__}\nligne : #{__LINE__}\n";
	 end
      end
      @f.print "
  }else{
    printf(\"erreur !\\n\");
  }
}";
    end
    @f.print "
void init_game(void){
  jsval rval;
  const char* champion_path;

  rt = JS_NewRuntime(8L * 1024L * 1024L);
  if (rt == NULL) return;

  champion_path = getenv(\"CHAMPION_PATH\");
  if (!champion_path)
      champion_path = \".\";
  snprintf(filename, 1024, \"%s/main.js\", champion_path);

  /* Create a context. */
  cx = JS_NewContext(rt, 8192);
  if (cx == NULL) return;
  JS_SetErrorReporter(cx, reportError);

  /* Create the global object. */
  global = JS_NewObject(cx, &global_class, NULL, NULL);
  if (global == NULL) return;

  module = JS_DefineObject(cx, global, MODULE_NAME, &module_class, NULL, JSPROP_ENUMERATE);
  if (module == NULL) return;
  if (!JS_InitStandardClasses(cx, global)) return;
  if (!JS_DefineFunctions(cx, module, myjs_module_functions)) return;
  if (!JS_DefineFunctions(cx, global, myjs_global_functions)) return;
  script = JS_CompileFile(cx, global, filename);
  if (script == NULL) return;
  if (!JS_ExecuteScript(cx, global, script, &rval)) return;
  re_init_game();
}

void end_game(void){
  re_end_game();
  JS_DestroyContext(cx);
  JS_DestroyRuntime(rt);
  JS_ShutDown();
}
"
    @f.close
  end

  def build
    @path = Pathname.new($install_path) + "js"
    @header_file = 'interface.h'
    @source_file = 'interface.c'
    generate_header
    generate_source
  end
end


class JsFileGenerator < FileGenerator
  def initialize
    super
    @lang = "js"
  end

  def print_comment(str)
    @f.puts '// ' + str if str
  end

  def print_multiline_comment(str)
    return unless str
    @f.puts "/*"
    str.each_line {|s| @f.print " * ", s }
    @f.puts ""," */"
  end

  def print_constant(type, name, val)
      @f.print "var  ", name, " = ", val, ";\n";
  end

  def print_proto(name, ret_type, args)
  end
    
  def generate_makefile
      target = $conf['conf']['player_lib']
      @f = File.open(@path + "Makefile", 'w')
      @f.print <<-EOF
  # -*- Makefile -*-

  lib_TARGETS = #{target}

  # Tu peux rajouter des fichiers sources, headers, ou changer
  # des flags de compilation.

  SRC       = #{@source_file} #{$conf["conf"]["player_filename"]}.js
  NAME      = #{$conf['conf']['player_lib']}.so

  #{target}-cflags = -ggdb3

  # Evite de toucher a ce qui suit
  #{target}-dists += #{@header_file}
  #{target}-srcs += ../includes/main.c
  include ../includes/makejs
      EOF
      @f.close
    end

  def build
    @path = Pathname.new($install_path) + "js"
    @js_file = $conf["conf"]["player_filename"] + ".js"

    # Build C interface files
    JsCFileGenerator.new.build

    # Generate perl source file
    @f = File.open(@path + @js_file, 'w')
    print_banner "generator_js.rb"
    build_constants
    @f.puts
    for_each_user_fun do |name, ret, args| 
      @f.print "function ", name, "()
{
  // code ici
}
"
    end
    @f.close
    generate_makefile
  end
end
