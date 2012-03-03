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

class HaskellFileGenerator < FileGenerator

  def initialize
    @lang = "Haskell"
    @types = {
      'void' => '',
      'int' => 'Int',
      'bool' => 'Bool' # Fixme
    }
  end

  def print_comment(str)
    @f.puts '-- ' + str if str
  end

  def print_multiline_comment(str)
    return unless str
    str.each {|s| @f.print '-- ', s }
    @f.puts
  end

  def print_constant(type, name, val)
      @f.print '_', name, " = ", val, "\n"
  end

  def print_proto(name, ret_type, args)
    @f.print "foreign import ccall " + name + " :: "
    first = true
    if args != nil and args != []
      args.each do |x|
        @f.print " -> " unless first
        @f.print conv_type(x[1])
        first = false
      end
    end
    if ret_type != nil
      @f.print " -> " unless first
      @f.print "IO " + conv_type(ret_type)
    end
    @f.puts
  end

  def generate_source
    @f = File.open(@path + @source_file, 'w')
    @f.puts "module #{@module_name} where", ""

    build_constants

    for_each_fun do |x, y, z| 
      print_proto(x, y, z)
    end

    for_each_user_fun do |name, y, z| 
      @f.puts "foreign export ccall " + name + ":: IO ()"
    end

    for_each_user_fun do |name, y, z| 
      @f.puts name + ":: IO ()"
      @f.puts name + " = do return () -- put your code here"
      @f.puts
    end

    @f.close
  end

  def generate_interface
    @f = File.open(@path + "interface.cc", 'w')
    @f.puts "
#include <stdio.h>
#include <HsFFI.h>

extern \"C\" {

#include \"#{$conf['conf']['player_filename']}_stub.h\"

extern void __stginit_#{@module_name} ( void );

struct HaskellVM
{
  HaskellVM(int argc, char** argv)
  {
    hs_init(&argc, &argv);
    hs_add_root(__stginit_#{@module_name});
  }

  ~HaskellVM()
  {
    hs_exit();
  }
};

void haskell_startup(int argc, char** argv)
{
  static HaskellVM vm(argc, argv);
}
}
"
    @f.close
  end

  def generate_makefile
    @f = File.open(@path + "Makefile", 'w')
    @f.print <<-EOF
#
# Makefile
#

SRCS      = #{@source_file} # Ajoutez ici vos fichiers .c
NAME      = #{$conf['conf']['player_lib']}.so

MY_CFLAGS = -ggdb3

include ../includes/makehaskell
    EOF
    @f.close
  end

  def build()
    @path = Pathname.new($install_path) + "haskell"
    @source_file = $conf['conf']['player_filename'] + '.hs'
    @module_name = $conf['conf']['player_filename'].capitalize

    generate_source
    generate_interface
    generate_makefile
  end

end
