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

# C++ generator, for lua-interface
class LuaCxxFileGenerator < CxxProto
  def initialize
    super
    @lang = "C++ (for lua interface)"
  end

  def generate_header()
    @f = File.open(@path + @header_file, 'w')
    print_banner "generator_lua.rb"

    @f.puts <<-EOF
#ifndef INTERFACE_HH_
# define INTERFACE_HH_

extern "C" {
# include <lua.h>
# include <lualib.h>
# include <lauxlib.h>
}

class LuaInterface
{
public:
  LuaInterface();
  ~LuaInterface();
  
  void callLuaFunction(const char* name);
  
private:
  lua_State* l_;
};

    EOF

    build_constants
    @f.puts "", 'extern "C" {', ""
    for_each_fun { |x, y, z| print_proto(x, y, z, "extern"); @f.puts ";" }
    for_each_user_fun { |x, y, z| print_proto(x, y, z); @f.puts ";" }
    @f.puts "}", "", "#endif // !INTERFACE_HH_"
    @f.close
  end

  def generate_source()
    @f = File.open(@path + @source_file, 'w')
    print_banner "generator_lua.rb"
    
    @f.puts <<-EOF
#include "interface.hh"

LuaInterface gl_lua;

/*
** Api functions called from LUA to stechec.
*/
extern "C" {
    EOF
      
    for_each_fun(false) do |name, ret, args|
      @f.print "int l_", name, "(lua_State* L)\n"
      @f.puts "{"
      var_args = []
      args.each_index do |i|
        n = (i + 1).to_s
        @f.print "  int arg", n, " = (int)lua_tonumber(L, ", n, ");\n"
        var_args << "arg" + n
      end if args
      @f.print "  int ret = (int)", name, "(", var_args.join(", "), ");\n"
      @f.puts "  lua_pushnumber(L, (double)ret);"
      @f.puts "  return 1;", "}"
    end

    @f.puts <<-EOF
} // !api functions

/*
** Api functions to register.
*/
static const struct binding_reg
{
  char *lua_name;
  int (*lua_func)(lua_State *L);
} binding_func[] = {
    EOF

    for_each_fun(false) do |name, ret, arg|
      @f.print '  {"', name, '", l_', name, "},"
    end

    @f.puts <<-EOF
  {NULL, NULL}
};

/*
** Load Lua code and register API.
*/
LuaInterface::LuaInterface()
{
  l_ = lua_open();
  luaL_openlibs(l_);

  /* Register the API bindings */
  fprintf(stderr, "[Lua] populating the lua table 'prologin'...\\n");
  lua_newtable(l_);
  for (int i = 0; binding_func[i].lua_name; i++)
    {
      lua_pushstring(l_, binding_func[i].lua_name);
      lua_pushcfunction(l_, binding_func[i].lua_func);
      lua_settable(l_, -3);
    }

  lua_setglobal(l_, "prologin");

  fprintf(stderr, "[Lua] loading lua code from lua_code.h...");
#include "lua_code.h"
  fprintf(stderr, " done.\\n");
}

LuaInterface::~LuaInterface()
{
  lua_close(l_);
}

/*
** Run a lua function.
*/
void LuaInterface::callLuaFunction(const char* name)
{
  lua_getglobal(l_, name);
  if (lua_pcall(l_, 0, 0, 0))
    {
      fprintf(stderr, "[Lua] Error running function %s: %s\\n",
              name, lua_tostring(l_, -1));
      lua_pop(l_, 1);  /* pop error message from the stack */
    }
}

/*
** Functions called from stechec to LUA.
*/
    EOF

    for_each_user_fun(false) do |name, type_ret, args|
      print_proto(name, type_ret, args, 'extern "C"')
      print_body "  gl_lua.callLuaFunction(\"" + name + "\");"
     end

    @f.close
  end

  def build
    @path = Pathname.new($install_path) + "lua"
    @header_file = 'interface.hh'
    @source_file = 'interface.cc'

    generate_header
    generate_source
  end

end

class LuaFileGenerator < FileGenerator
  def initialize
    @lang = "lua"
  end

  def print_comment(str)
    @f.puts '-- ' + str if str
  end

  def print_multiline_comment(str)
    return unless str
    str.each {|s| @f.print '-- ', s }
    @f.puts ""
  end

  # print a constant
  def print_constant(type, name, val)
      @f.print '  public static final int ', name, ' = ', val, ";\n"
  end

  def generate_makefile
    @f = File.open(@path + "Makefile", 'w')
    @f.print <<-EOF
# -*- Makefile -*-
#

SRC       = #{@source_file} # Ajoutez ici vos fichiers .lua
NAME      = #{$conf['conf']['player_lib']}.so

MY_CXXFLAGS = -ggdb3

include ../includes/makelua
    EOF
    @f.close
  end


  def build()
    @path = Pathname.new($install_path) + "lua"
    @source_file = $conf['conf']['player_filename'] + '.lua'

    ######################################
    ##  interface.hh file generating    ##
    ######################################
    LuaCxxFileGenerator.new.build	
    
    ######################################
    ##  prologin.lua file generating    ##
    ######################################
    @f = File.new(@path + @source_file, 'w')
    print_banner "generator_lua.rb"
    for_each_user_fun do |name, ret, args|
      @f.puts "function " + name + "()"
      @f.puts "    -- Place ton code ici", "end", ""
    end
    @f.close

    generate_makefile
  
    # must copy the file bin2c.lua
    if File.exist? 'files/bin2c.lua'
      File.copy 'files/bin2c.lua', @path.to_s
    else
      File.copy Pathname.new(PKGDATADIR) + 'files' + 'bin2c.lua', @path.to_s
    end
  end

end
