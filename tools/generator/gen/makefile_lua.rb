#
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

class LuaMakefile
  def initialize
    @makefile = <<EOF
#####################
# Macro Definitions #
#####################
LUA_VER	= 5.1
LUA	= lua$(LUA_VER)
CXX 	= g++
LIMEOBJ ?= ../includes/main.o
OBJS 	= interface.o $(LIMEOBJ)
RM 	= /bin/rm -f

CFLAGS = -fPIC
CXXFLAGS = -fPIC -W -Wall \\
   $(shell pkg-config --cflags lua$(LUA_VER)) \\
   $(MY_CXXFLAGS)

LIBS = $(shell pkg-config --libs lua$(LUA_VER))

##############################
# Basic Compile Instructions #
##############################

all: $(NAME)

# generated c-code from lua
lua_code.h: $(SRC) bin2c.lua Makefile
\t$(LUA) bin2c.lua $(SRC) > $@

interface.o: interface.cc lua_code.h interface.hh

$(NAME): $(OBJS)
\t$(CXX) #{TARGET_GCC_LINK} $(OBJS) $(LIBS) -o $(NAME) $(CHECK_CHEAT)
\t@echo Finished

clean:
\t@echo "Cleaning..."
\t$(RM) $(OBJS) *~ lua_code.h \\#*\\#

distclean: clean
\t$(RM) $(NAME) prologin.tgz

tar:
\ttar cvzf prologin.tgz *.{cc,hh,lua}
EOF
  end

  def build_client(path)
    f = File.new(path + "makelua", 'w')
    f.puts "# -*- Makefile -*-"
    f.print @makefile
    f.close
  end

  def build_metaserver(path)
    f = File.new(path + "Makefile-lua", 'w')
    f.print <<-EOF
# -*- Makefile -*-

# $(wildcard *.lua)
SRC       = prologin.lua 
NAME      = #{$conf['conf']['player_lib']}.so

MY_CXXFLAGS = -O2

LIMEOBJ = stechec_main.o

    EOF
    f.print @makefile
    f.close
  end

end
