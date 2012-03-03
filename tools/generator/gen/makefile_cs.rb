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

class CSharpMakefile
  def build_metaserver(path)
    f = File.new(path + "Makefile-cs", 'w')
    target = $conf['conf']['player_lib']
    f.print <<-EOF
# -*- Makefile -*-

lib_TARGETS = #{target}

#{target}-srcs = $(wildcard *.cs)

#{target}-dists = interface.hh
#{target}-srcs += api.cs interface.cc stechec_main.cc
#{target}-cxxflags = -fPIC \
    $(shell pkg-config --cflags glib-2.0 mono)
#{target}-ldflags = -lm \
    $(shell pkg-config --libs glib-2.0 mono)
#{target}-csflags = -target:library -nowarn:0169,0649

V=1
include $(MFPATH)/rules.mk
    EOF
    f.close
  end

end
