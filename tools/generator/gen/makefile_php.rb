#
# Stechec project is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# The complete GNU General Public Licence Notice can be found as the
# `NOTICE' file in the root directory.
#
# Copyright (C) 2010 Prologin
#

class PhpMakefile
  def build_metaserver(path)
    target = $conf['conf']['player_lib']
    f = File.open(path + 'Makefile-php', 'w')
    f.print <<-EOF
# -*- Makefile -*-

lib_TARGETS = #{target}

# Tu peux rajouter des fichiers sources ici
#{target}-dists = $(wildcard *.php)

# Evite de toucher a ce qui suit
#{target}-dists += interface.hh
#{target}-srcs = interface.cc stechec_main.cc
#{target}-cxxflags = -fPIC $(shell php-config --includes) -Wno-write-strings
#{target}-ldflags = -s $(shell php-config --libs --ldflags) -lphp5

V=1
include $(MFPATH)/rules.mk
EOF
    f.close
  end
end
