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

class PascalMakefile
  def initialize
    @makefile = <<EOF
#####################
# Macro Definitions #
#####################
CXX 	  = g++
FPC       = fpc
MAKE 	  = make
OBJS 	  = ${SRC:.pas=.o}
LIMEOBJ   ?= ../includes/main.o
RM 	  = /bin/rm -f

CFLAGS    = -fPIC $(MY_CFLAGS)
CXXFLAGS  = -fPIC $(MY_CXXFLAGS)

##############################
# Basic Compile Instructions #
##############################

.SUFFIXES: .pas

all: ${NAME}

$(NAME): interface.o $(SRC) $(LIMEOBJ)
\t${FPC} -Cn prologin.pas ${CFLAGS} -o"${NAME}"
#\tfpc, what a bullshit ! it doesn't want our ~2000 command line argument...
#\tinstead, omit link phase, add options for ld in his script, then link.
\t@sed -ie 's!^/usr/bin/ld!& interface.o $(LIMEOBJ) !' ./ppas.sh
#\t@sed -ie '/INPUT(/ a $(LIMEOBJ) ' ./link.res
\t@sed -ie '/global:/ a run;' ./link.res
\t@./ppas.sh
\t@$(RM) ./ppas.sh ./link.res*
\t\#@mv lib${NAME} ${NAME}
\t@echo Finished

clean:
\t@echo "Cleaning..."
\t${RM} *.o *.ppu *~ \#*\#

distclean: clean
\t${RM} ${NAME} #{$conf['conf']['player_filename']}.tgz

tar:
\ttar cvzf #{$conf['conf']['player_filename']}.tgz prolo_interface.pas $(SRC) $(AUXFILES)
EOF

  end

  def build_client(path)
    f = File.new(path + "makepascal", 'w')
    f.puts "# -*- Makefile -*-"
    f.print @makefile
    f.close
  end

  def build_metaserver(path)
    f = File.new(path + "Makefile-pascal", 'w')
    f.print <<-EOF
# -*- Makefile -*-

SRC       = $(wildcard *.pas)
NAME      = #{$conf['conf']['player_lib']}.so

CHECK_CHEAT = `while read i; do echo -wrap $$i; done < forbidden_fun-pascal`
LIMEOBJ = stechec_main.o

list-run-reqs:
\t@true

    EOF
    f.print @makefile
    f.close
  end

end
