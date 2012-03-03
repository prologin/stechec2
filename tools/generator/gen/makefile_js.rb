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


class JsMakefile
    def initialize
    @makefile = <<-EOF
#####################
# Macro Definitions #
#####################
CC 	= gcc
LIMEOBJ ?= ../includes/main.o
OBJS 	= interface.o $(LIMEOBJ)
RM 	= /bin/rm -f
AUXFILES= main.js

CFLAGS = `pkg-config --cflags mozilla-js` $(MY_CFLAGS)

LIBS = `pkg-config --libs mozilla-js`

##############################
# Basic Compile Instructions #
##############################

all: $(NAME)

interface.o : interface.c interface.h

$(NAME): $(OBJS)
\t$(CC) #{TARGET_GCC_LINK} $(OBJS) $(LIBS) -o $(NAME)
\t@echo Finished

clean:
\t@echo "Cleaning..."

distclean: clean
\t$(RM) $(NAME) champion.tgz

tar:
\ttar cvzf champion.tgz $(AUXFILES) interface.c interface.h $(SRC)
EOF
  end

  def build_client(path)
    f = File.new(path + "makejs", 'w')
    f.puts "# -*- Makefile -*-"
    f.print @makefile
    f.close
  end

  def build_metaserver(path)
    f = File.new(path + "Makefile-js", 'w')
    f.print <<-EOF
# -*- Makefile -*-

SRC       = $(wildcard *.js)
NAME      = #{$conf['conf']['player_lib']}.so

MY_CFLAGS = -O2

LIMEOBJ   = stechec_main.o

    EOF
    f.print @makefile
    f.close
  end

end
