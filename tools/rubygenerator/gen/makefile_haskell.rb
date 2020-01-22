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
  def build_metaserver(path)
    f = File.new(path + "Makefile-haskell", 'w')
    target = $conf['conf']['player_lib']
    f.puts "# -*- Makefile -*-"
    f.puts <<-EOF
lib_TARGETS = #{target}
#{target}-srcs = $(wildcard *.hs) $(wildcard *.hsc) $(wildcard *.cc)

V=1

STECHEC_LANG = haskell
include $(MFPATH)/rules.mk
EOF
    f.print @makefile
    f.close
  end
end
