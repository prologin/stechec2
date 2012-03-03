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

class CxxMakefile
  def build_metaserver(path)
    f = File.new(path + "Makefile-cxx", 'w')
    target = $conf['conf']['player_lib']
    f.print <<-EOF
# -*- Makefile -*-

lib_TARGETS = #{target}

# Tu peux rajouter des fichiers sources, headers, ou changer
# des flags de compilation.
#{target}-srcs = $(wildcard *.cc)
#{target}-cxxflags = -O2 -Wall

V=1
include $(MFPATH)/rules.mk
    EOF
    f.close
  end

end
