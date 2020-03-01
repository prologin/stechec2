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
#{target}-srcs = interface.cc

py_config = $(shell python3-config --embed >/dev/null && echo python3-config --embed || echo python3-config )
#{target}-cxxflags = -O2 -fPIC $(shell $(py_config) --includes)
#{target}-ldflags = -s $(shell $(py_config) --ldflags)

V=1
include $(MFPATH)/rules.mk
    EOF
    f.close
  end

end
