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

  def build_structs_operators
    @f.puts <<-EOF
// Les fonctions suivantes définissent les opérations de comparaison, d'égalité
// et de hachage sur les structures du sujet.

namespace std {
  template <typename T>
  struct hash<std::vector<T>>
  {
    std::size_t operator()(const std::vector<T>& v)
    {
      std::size_t res = v.size();
      for (const auto& e : v)
        res ^= std::hash<T>()(e) + 0x9e3779b9 + (res << 6) + (res >> 2);
      return res;
    }
  };
}

EOF

    for_each_struct(false) do |x|
      @f.print "inline bool operator==("
      @f.puts "const #{x['str_name']}& a, const #{x['str_name']}& b) {"
      x['str_field'].each do |f|
          @f.puts "  if (a.#{f[0]} != b.#{f[0]}) return false;"
      end
      @f.puts "  return true;"
      @f.puts "}\n\n"

      @f.print "inline bool operator!=("
      @f.puts "const #{x['str_name']}& a, const #{x['str_name']}& b) {"
      x['str_field'].each do |f|
          @f.puts "  if (a.#{f[0]} != b.#{f[0]}) return true;"
      end
      @f.puts "  return false;"
      @f.puts "}\n\n"

      @f.print "inline bool operator<("
      @f.puts "const #{x['str_name']}& a, const #{x['str_name']}& b) {"
      x['str_field'].each do |f|
          @f.puts "  if (a.#{f[0]} < b.#{f[0]}) return true;"
          @f.puts "  if (a.#{f[0]} > b.#{f[0]}) return false;"
      end
      @f.puts "  return false;"
      @f.puts "}\n\n"

      @f.print "inline bool operator>("
      @f.puts "const #{x['str_name']}& a, const #{x['str_name']}& b) {"
      x['str_field'].each do |f|
          @f.puts "  if (a.#{f[0]} > b.#{f[0]}) return true;"
          @f.puts "  if (a.#{f[0]} < b.#{f[0]}) return false;"
      end
      @f.puts "  return false;"
      @f.puts "}\n\n"

      @f.puts <<-EOF
namespace std {
  template <>
  struct hash<#{x['str_name']}>
  {
    std::size_t operator()(const #{x['str_name']}& s)
    {
      std::size_t res = 0;
EOF
      x['str_field'].each do |f|
          @f.print "      res ^= 0x9e3779b9 + (res << 6) + (res >> 2) + ";
          @f.puts "std::hash<#{conv_type(@types[f[1]])}>()(s.#{f[0]});"
      end
      @f.puts <<-EOF
      return res;
    }
  };
}

EOF
    end
  end

  def generate_header()
    @f = File.open(@path + @header_file, 'w')
    print_banner "generator_cxx.rb"
    @f.puts "#ifndef PROLOGIN_HH_", "# define PROLOGIN_HH_", ""
    @f.puts "# include <functional>", ""
    @f.puts "# include <string>", ""
    @f.puts "# include <vector>", ""
    build_constants
    build_enums
    build_structs
    build_inlines
    build_structs_operators
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
#{target}-cxxflags = -ggdb3 -Wall -std=c++11

# Evite de toucher a ce qui suit
#{target}-dists += #{@header_file}
STECHEC_LANG=cxx
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
