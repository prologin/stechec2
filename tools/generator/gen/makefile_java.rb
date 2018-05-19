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

class JavaMakefile
  def build_metaserver(path)
    f = File.new(path + "Makefile-java", 'w')
    target = $conf['conf']['player_lib']
    f.print <<-EOF
# -*- Makefile -*-

JAVA_HOME ?= $(shell readlink -f /usr/bin/java | sed "s:/jre/bin/java$$::")
lib_TARGETS = #{target}

#{target}-srcs = Interface.java Prologin.java $(wildcard *.java)
#{target}-cxxflags = -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -O2
#{target}-ldflags = -Wl,-rpath -Wl,$(JAVA_HOME)/jre/lib/amd64/server/ -L$(JAVA_HOME)/jre/lib/amd64/server/ -ljvm

#{target}-dists = interface.hh
#{target}-srcs += interface.cc

V=1
include $(MFPATH)/rules.mk
EOF
    f.close
  end

end
