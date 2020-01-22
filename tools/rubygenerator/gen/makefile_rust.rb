#
# Stechec project is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# The complete GNU General Public Licence Notice can be found as the
# `NOTICE' file in the root directory.
#
# Copyright (C) 2005, 2006, 2018 Prologin
#

class RustMakefile
  def build_metaserver(path)
    f = File.new(path + "Makefile-rust", 'w')
    target = $conf['conf']['player_lib']
    f.print <<-EOF
# -*- Makefile -*-

lib_TARGETS = #{target}

#{target}-srcs = interface.cc #{$conf['conf']['player_filename']}.rs
#{target}-rustcflags = -O
#{target}-ldflags = -fPIC -Wl,--whole-archive
#{target}-ldlibs  = -Wl,--no-whole-archive -lm -lrt -ldl -lpthread -lstdc++

V=1
include $(MFPATH)/rules.mk
    EOF
    f.close
  end

end
