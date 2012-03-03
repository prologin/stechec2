#  -*- ruby -*-
# Stechec project is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# The complete GNU General Public Licence Notice can be found as the
# `NOTICE' file in the root directory.
#
# Copyright (C) 2005, 2006, 2007 Prologin
#

class CxxFileGenerator < CxxProto

  def build_inlines
    for_each_fun do |fn|
      @f.puts cxx_proto(fn, "api_", "extern \"C\"") + ";"
      @f.puts cxx_proto(fn, "", "static inline")
      @f.puts "{"
      @f.print "  "
      unless fn.ret.is_nil?
        @f.print "return "
      end
     @f.print "api_", fn.name, "("
     args = fn.args.map { |arg| arg.name }
     @f.print args.join(", "), ");\n"
     @f.puts "}"
     @f.puts
    end
  end

  def generate_header()
    @f = File.open(@path + @header_file, 'w')
    print_banner "generator_cxx.rb"
    @f.puts "#ifndef PROLOGIN_HH_", "# define PROLOGIN_HH_", ""
    @f.puts "# include <vector>", ""
    @f.puts "# include <string>", ""
    build_constants
    build_enums
    build_structs
    build_inlines
    @f.puts "", 'extern "C" {', ""
    for_each_user_fun do |fn|
      @f.print cxx_proto(fn)
      @f.puts ";"
    end
    @f.puts "}"
    @f.puts "#endif"
    @f.close
  end

  def generate_source()
    @f = File.open(@path + @source_file, 'w')
    print_banner "generator_cxx.rb"
    print_include @header_file
    @f.puts
    for_each_user_fun do |fn| 
      @f.print cxx_proto(fn)
      print_body "  // fonction a completer"
    end
    @f.close
  end

  def generate_makefile
    target = $conf['conf']['player_lib']
    @f = File.open(@path + "Makefile", 'w')
    @f.print <<-EOF
# -*- Makefile -*-

lib_TARGETS = #{target}

# Tu peux rajouter des fichiers sources, headers, ou changer
# des flags de compilation.
#{target}-srcs = #{@source_file}
#{target}-dists =
#{target}-cxxflags = -ggdb3 -Wall

# Evite de toucher a ce qui suit
#{target}-dists += #{@header_file}
#{target}-srcs += ../includes/main.cc
include ../includes/rules.mk
    EOF
    @f.close
  end

  def build()
    @path = Pathname.new($install_path) + "cxx"
    @header_file = $conf['conf']['player_filename'] + '.hh'
    @source_file = $conf['conf']['player_filename'] + '.cc'

    generate_header
    generate_source
    generate_makefile
  end

end
