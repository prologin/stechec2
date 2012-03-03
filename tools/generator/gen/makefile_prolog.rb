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
#

class PrologMakefile

def initialize
    @makefile = <<-EOF
# -*- Makefile -*-
all:
	plld -o interface interface.cc ../includes/main.cc \$SRC -shared
	ld -lpl interface.so -fPIC -shared -o champion.so

EOF
end
def build_client(path)
    f = File.new(path + "makeprolog", 'w')
    f.puts "# -*- Makefile -*-"
    f.print @makefile
    f.close
  end

  def build_metaserver(path)
    f = File.new(path + "Makefile-prolog", 'w')
    target = $conf['conf']['player_lib']
    f.print @makefile
    f.close
  end

end
