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

class PythonMakefile
  def build_metaserver(path)
    target = $conf['conf']['player_lib']
    f = File.open(path + "Makefile-python", 'w')
    f.print <<-EOF
# -*- Makefile -*-

lib_TARGETS = #{target}

# Tu peux rajouter des fichiers source ici
#{target}-dists = $(wildcard *.py)

# Evite de toucher a ce qui suit
#{target}-dists += interface.hh
#{target}-srcs = interface.cc stechec_main.cc

pc = $(shell for p in python2-config python-config; do if which $$p &>/dev/null; then echo $$p; fi done | head -n 1)
#{target}-cxxflags = -fPIC $(shell $(pc) --includes)
#{target}-ldflags = -s $(shell $(pc) --ldflags)

V=1
include $(MFPATH)/rules.mk
    EOF
    f.close
  end

end
