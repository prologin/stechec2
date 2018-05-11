# -*- ruby -*-
# Stechec project is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# The complete GNU General Public Licence Notice can be found as the
# `COPYING' file in the root directory.
#
# Copyright (C) 2005, 2006, 2007 Prologin
#

if ARGV[0] == "--version"
  print <<EOF
generator 3.0

Copyright (C) 2005, 2006, 2007 Prologin.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

Written by Olivier Gournet.
EOF
  exit 0

elsif (ARGV.length < 3 or ARGV[0] == "--help" or ARGV[0] == "-h")
  print <<EOF
\`generator\' is part of \`stechec\'. Its goal is to generate source
code based on a yaml description file. This avoid maintaining "n * l"
source, where "n" is the number of contest and "l" the number of langages
available.

Usage: stechec2-generator <option> <yaml_file> <install_path>

Options:
  player       create startup files, for candidats and for all langages.
  server       create special Makefiles for worker nodes.
  rules        generate some boring piece for api rules.
  apidoc       generate latex API documentation for the subject.
  sphinxdoc    generate sphinx API documentation for the subject.

Examples:
  stechec2-generator player ant /tmp/
  stechec2-generator apidoc /goinfre/titi.yaml foo/

Report bugs to <info@prologin.org>
EOF
  exit 0
end


require 'yaml'
require 'fileutils'
require 'pathname'
require 'conf'

$languages = %w[c cs cxx caml java python php rust haskell]

def make_player
  $languages.each do |x|
    require "gen/generator_" + x
  end

  # create directories
  install_path = Pathname.new($install_path)
  $languages.each do |x|
    (install_path + x).mkpath
  end

  # PrologFileGenerator.new.build

  CFileGenerator.new.build
  CSharpFileGenerator.new.build
#  PascalFileGenerator.new.build
  CxxFileGenerator.new.build
  CamlFileGenerator.new.build
  JavaFileGenerator.new.build
  HaskellFileGenerator.new.build
#  RubyFileGenerator.new.build
  RustFileGenerator.new.build
#  LuaFileGenerator.new.build
  PythonFileGenerator.new.build
  PhpFileGenerator.new.build
#  JsFileGenerator.new.build
end


def make_includes
  $languages.each do |x|
    require "gen/makefile_" + x
  end
  # create directory
  install_path = Pathname.new($install_path) + "includes"
  install_path.mkpath

  FileUtils.cp Pathname.new(PKGDATADIR) + 'files/README', 'README'
  # copy rules.mk and toposort.py into includes.
  if Pathname.new('files/rules.mk').exist?
    FileUtils.cp 'files/rules.mk', install_path.to_s
    FileUtils.cp 'files/toposort.py', install_path.to_s
  else
    FileUtils.cp Pathname.new(PKGDATADIR) + 'files/rules.mk', install_path.to_s
    FileUtils.cp Pathname.new(PKGDATADIR) + 'files/toposort.py', install_path.to_s
  end

  # these 7 are not needed anymore for client, it uses rules.mk
#   CMakefile.new.build_client(install_path)
#   CxxMakefile.new.build_client(install_path)
#   JavaMakefile.new.build_client(install_path)
#   CamlMakefile.new.build_client(install_path)
#   PhpMakefile.new.build_client(install_path)
#   PythonMakefile.new.build_client(install_path)
#   CSharpMakefile.new.build_client(install_path)
#   PascalMakefile.new.build_client(install_path)
#   PrologMakefile.new.build_client(install_path)
#   RubyMakefile.new.build_client(install_path)
#   RustMakefile.new.build_client(install_path)
#   JsMakefile.new.build_client(install_path)
#   LuaMakefile.new.build_client(install_path)
end

def make_server
  $languages.each do |x|
    require "gen/makefile_" + x
  end

  # create directory
  install_path = Pathname.new($install_path)
  install_path.mkpath

  CMakefile.new.build_metaserver(install_path)
  CSharpMakefile.new.build_metaserver(install_path)
  CxxMakefile.new.build_metaserver(install_path)
  JavaMakefile.new.build_metaserver(install_path)
  # PascalMakefile.new.build_metaserver(install_path)
  CamlMakefile.new.build_metaserver(install_path)
  HaskellMakefile.new.build_metaserver(install_path)
  # LuaMakefile.new.build_metaserver(install_path)
  PythonMakefile.new.build_metaserver(install_path)
  PhpMakefile.new.build_metaserver(install_path)
  # PrologMakefile.new.build_metaserver(install_path)
  # RubyMakefile.new.build_metaserver(install_path) TODO
  RustMakefile.new.build_metaserver(install_path)
  # JsMakefile.new.build_metaserver(install_path) TODO
  # copy some used files
  path = Pathname.new(PKGDATADIR) + "files"
  FileUtils.cp((path + "toposort.py").to_s, install_path.to_s)
  FileUtils.cp((path + "rules.mk").to_s, install_path.to_s)
end

# opens YAML file and parses it
case ARGV[1]
when /\//
  $conf = YAML::load(File.open(ARGV[1]))
else
  path = Pathname.new(PKGDATADIR) + ARGV[1]
  $conf = YAML::load(File.open(path + (ARGV[1] + ".yml")))
end
$install_path = ARGV[2]

if $conf["conf"] == nil then
  $conf["conf"] = Hash.new
  $conf["conf"]["package_name"] = $conf["name"]
  $conf["conf"]["player_lib"] = "champion"
  $conf["conf"]["player_filename"] = "prologin"
end

$conf["constant"] = Hash.new unless $conf["constant"]
$conf["enum"] = Hash.new unless $conf["enum"]
$conf["function"] = Hash.new unless $conf["function"]
$conf["struct"] = Hash.new unless $conf["struct"]

# detect os, for some specificities in Makefile
case `uname`
when /Darwin/
  TARGET_OS=:darwin
  TARGET_GCC_LINK='-bundle -undefined dynamic_lookup'
else
  TARGET_OS=:linux
  TARGET_GCC_LINK='-shared'
end


case ARGV[0]
when "rules"
  require "gen/make_rules"

when "player"
  make_player
  make_includes

when "server"
  make_server

when "apidoc"
  require 'erb'

  s = File.read(get_file_path('gen/make_tex.rb', PKGRUBYDIR))
  path = Pathname.new($install_path)
  path.mkpath
  File.open(path + "apidoc.tex", 'w') do |f|
    f.puts ERB.new(s, nil, '<-%->', '$erbout_').result
  end

  FileUtils.cp Pathname.new(PKGDATADIR) + 'files/useapi.tex', path+'useapi.tex'


when "sphinxdoc"
  require 'erb'

  s = File.read(get_file_path('gen/make_sphinx.rb', PKGRUBYDIR))
  path = Pathname.new($install_path)
  path.mkpath
  File.open(path + "file.rst", 'w') do |f|
    f.puts ERB.new(s, nil, '<-%->', '$erbout_').result
  end

else
  puts "Bad argument '#{ARGV[0]}'. try: generator --help"
  exit 1
end
