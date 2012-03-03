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

class HaskellMakefile

  def initialize
    @makefile = <<-EOF
#####################
# Macro Definitions #
#####################
CXX		= g++
GHC		= ghc6
INTERF_OBJS	= interface.o $(SRCS:.hs=_stub.o) 
OBJS 		= ${SRCS:.hs=.o}
RM		= /bin/rm -f
# -no-hs-main -dynamic

INCLUDES	= -I/usr/lib/ghc-6.4/include
CFLAGS = -fPIC
CXXFLAGS	= -fPIC -W -Wall $(INCLUDES)

PRETEND_UNDEF	= -u GHCziBase_Izh_static_info -u GHCziBase_Czh_static_info -u GHCziFloat_Fzh_static_info -u GHCziFloat_Dzh_static_info -u GHCziPtr_Ptr_static_info -u GHCziWord_Wzh_static_info -u GHCziInt_I8zh_static_info -u GHCziInt_I16zh_static_info -u GHCziInt_I32zh_static_info -u GHCziInt_I64zh_static_info -u GHCziWord_W8zh_static_info -u GHCziWord_W16zh_static_info -u GHCziWord_W32zh_static_info -u GHCziWord_W64zh_static_info -u GHCziStable_StablePtr_static_info -u GHCziBase_Izh_con_info -u GHCziBase_Czh_con_info -u GHCziFloat_Fzh_con_info -u GHCziFloat_Dzh_con_info -u GHCziPtr_Ptr_con_info -u GHCziPtr_FunPtr_con_info -u GHCziStable_StablePtr_con_info -u GHCziBase_False_closure -u GHCziBase_True_closure -u GHCziPack_unpackCString_closure -u GHCziIOBase_stackOverflow_closure -u GHCziIOBase_heapOverflow_closure -u GHCziIOBase_NonTermination_closure -u GHCziIOBase_BlockedOnDeadMVar_closure -u GHCziIOBase_BlockedIndefinitely_closure -u GHCziIOBase_Deadlock_closure -u GHCziWeak_runFinalizzerBatch_closure -u __stginit_Prelude

##############################
# Basic Compile Instructions #
##############################

all: ${NAME}

${NAME}: $(INCL) ${OBJS} $(INTERF_OBJS)
\t$(CXX) -shared  ${OBJS} $(INTERF_OBJS) -L/usr/lib/ghc-6.4 -lHShaskell98 -lHSbase -lHSbase_cbits -lHSrts -lm -lgmp -ldl $(PRETEND_UNDEF) -o ${NAME}
\t@echo Finished

clean:
\t@echo "Cleaning..."
\t${RM} *.o *~ *.hi \#*\# 

distclean: clean
\t${RM} ${NAME} #{$conf['conf']['player_filename']}.tgz

%.o	: %.hs
\t$(GHC) -c -fPIC -fglasgow-exts $< -o $@

tar:
\ttar cvzf #{$conf['conf']['player_filename']}.tgz $(SRCS) interface.cc
    EOF
  end

  def build_client(path)
    f = File.new(path + "makehaskell", 'w')
    f.puts "# -*- Makefile -*-"
    f.print @makefile
    f.close
  end

  def build_metaserver(path)
    f = File.new(path + "Makefile-haskell", 'w')
    f.print <<-EOF
# -*- Makefile -*-

SRCS      = $(wildcard *.hs)
NAME      = #{$conf['conf']['player_lib']}.so

MY_CFLAGS = -O2

# FIXME
#CHECK_CHEAT = `while read i; do echo -wrap $$i; done < forbidden_fun-c`

    EOF
    f.print @makefile
    f.close
  end

end
