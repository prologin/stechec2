# -*- coding: iso-8859-1 -*-
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

#
# Cree un template pour les regles d'un nouveau concours, decrit par
# un fichier YAML
# Appelé depuis generator.rb
#

require "gen/generator_c.rb"

# cree le repertoire et detare la tarball dedans
package_name = $conf['conf']['package_name']
librule_name = $conf['conf']['rule_lib']
install_path = Pathname.new($install_path) + package_name + "rules"
install_path.mkpath

# Copie les quelques squelettes.
files = %w{
  Api.cc
  Api.hh
  Client.cc
  Client.hh
  ClientDiffer.cc
  ClientDiffer.hh
  ClientEntry.cc
  ClientEntry.hh
  Constant.hh
  GameData.cc
  GameData.hh
  Makefile.am
  Server.cc
  Server.hh
  ServerEntry.cc
  ServerEntry.hh
  ServerResolver.cc
  ServerResolver.hh
  check.cc
  interface.cc
  load_rules.cc
}
files.each do |x|
  if File.exist? 'files/' + x
    FileUtils.cp 'files/' + x, install_path.to_s
  else
    FileUtils.cp Pathname.new(PKGDATADIR) + 'files' + x, install_path.to_s
  end
end

# genere les entrees pour les autres fichiers

puts "Generating API files from YAML."
Dir.chdir(install_path)

class CxxFileGenerator < CxxProto
  def replace(line)
    replaces = {}
    replaces['!!year!!'] = Date.today.year.to_s
    replaces['!!package_name!!'] = $conf['conf']['package_name']
    replaces['!!rule_lib!!'] = $conf['conf']['server_rule_lib']
    replaces.each do |key, value|
      line = line.sub(key, value)
    end
    line
  end

  def fill_file_section(filename, &block)
    puts "Cook #{filename}..."
    FileUtils.mv(filename, filename + ".tmp")
    File.open(filename + ".tmp" , 'r') do |fr|
      @f = File.new( filename, 'w')
      while s = fr.gets do
        if s =~ /@@GEN_HERE@@/ then
          block.call
        else
          @f.puts s
        end
      end
      @f.close
    end
    File.unlink(filename + ".tmp")
  end

  def expand_variables(filename)
    puts "Expanding #{filename}..."
    FileUtils.mv(filename, filename + ".tmp")
    File.open(filename + ".tmp" , 'r') do |fr|
      @f = File.new( filename, 'w')
      while s = fr.gets do
        if s =~ /!!.*!!/ then
          @f.puts(replace s)
        else
          @f.puts s
        end
      end
      @f.close
    end
    File.unlink(filename + ".tmp")
  end

  # Api.cc
  def print_cxx_api
    for_each_fun do |fn|
      if not fn.dumps then
        @f.print cxx_proto(fn, "Api::")
        @f.puts "
{
  // TODO
  abort();
}
"
      end
    end
  end



  def convert_to_string_arr(ty)
    @f.print <<-EOF
std::string convert_to_string(std::vector<#{ty}> in){
  if (in.size()){
    std::string s = "[" + convert_to_string(in[0]);
    for (int i = 1, l = in.size(); i < l; i++){
      s = s + ", " + convert_to_string(in[i]);
    }
    return s + "]";
  }else{
    return "[]";
  }
}
EOF
  end


  # interface.cc
  def print_interface
    # print info
    # string convertion fonctions (util function for "print info")
    @f.print <<-EOF

// todo avoir un ostringstream a la place de std::string

std::string convert_to_string(int i){
  std::ostringstream s;
  s << i;
  std::string result = s.str();
  return result;
}
std::string convert_to_string(std::string i){
  return i;
}
std::string convert_to_string(bool i){
  return i?"true":"false";
}
EOF

convert_to_string_arr "int";

    # string conversion functions for enum
    for_each_enum(false) do |e|
      ty = e["enum_name"]
      @f.puts "std::string convert_to_string(#{ty} in){"
      @f.puts "  switch (in)\n  {"
      e["enum_field"].each do |f|
        @f.puts "    case #{f[0].upcase()}: return \"\\\"#{f[0]}\\\"\";"
      end
      @f.puts "  }"
      @f.puts "  return \"bad value\";"
      @f.puts "}"
      convert_to_string_arr ty
    end

    # string conversion functions for vectors
    for_each_struct(false) do |e|
      ty = e["str_name"]
      @f.puts "std::string convert_to_string(#{ty} in){"
      args = []
      e["str_field"].each do |e|
        @f.puts "  std::string #{e[0]} = convert_to_string(in.#{e[0]});"
	args.push "\"#{e[0]}:\" + #{e[0]}"
      end
      @f.puts "  std::string out = \"{\";"
      virgule = false;
      args.each do |s|
        if virgule then @f.puts "  out += \", \";" end
        @f.puts "  out += #{s};"
        virgule = true;
      end
      @f.puts "  return out + \"}\";"
      @f.puts "}"
      @f.puts
      convert_to_string_arr ty
    end

    # dump infos function
    @f.puts "std::string dump_infos()","{"
    i = 0
    args_string = []
    for_each_info do |x|
      i = i + 1
      ty = x['type']
      if ty.is_array? then
        ty = "std::vector<#{ty.type.name}>"
      else
        ty = ty.name
      end
      args_ = x['fct_arg'].map do |x| x end
      args_str = args_.join ", "
      @f.puts "  std::string string_info#{i} = convert_to_string(api->#{x['fct_name']}(#{args_str}));"
      args_string.push("string_info#{i}")
    end
    @f.puts "  // TODO modifie les infos ici, si besoin (si tu as une vue subjective, ca peut-etre utile)"

    @f.puts "  std::string out = \"[\";"
    if args_string != [] then @f.puts "  out += #{args_string.join " + \", \" + " };" end
    @f.puts "  return out + \"]\";"
    @f.puts "}"
    for_each_fun true, "function" do |fn| 
      if fn.dumps then
        t = fn.dumps
        @f.puts "std::ostream& operator<<(std::ostream& os, #{t.name} v)"
        @f.puts "{"
        if t.is_enum?
          @f.puts "  switch (v) {"
          t.conf['enum_field'].each do |f|
            f = f[0].upcase
            @f.puts "  case #{f}: os << \"#{f}\"; break;"
          end
          @f.puts "  }"
        else
          @f.puts "  os << \"{ \";"
          t.conf['str_field'].each do |f|
            if f != t.conf['str_field'][0]
              @f.puts "  os << \", \";"
            end
            ty = @types[f[1]]
            na = f[0]
            @f.puts "  os << \"#{na}\" << \"=\" << v.#{na};"
          end
          @f.puts "  os << \" }\";"
        end
        @f.puts "  return os;"
        @f.puts "}"

        @f.print cxx_proto(fn, "api_", 'extern "C"')
        @f.puts "", "{"
        @f.puts "  std::cerr << v << std::endl;"
        @f.puts "}"
      else
        @f.print cxx_proto(fn, "api_", 'extern "C"')
        @f.puts "", "{"
        @f.print "  "
        unless fn.ret.is_nil? then
          @f.print "return "
        end
        @f.print "api->#{fn.name}("
        args = fn.args
        if args != []
          args[0..-2].each do |arg|
            @f.print arg.name, ", "
          end
          @f.print args[-1].name
        end
        @f.puts ");"
        @f.puts "}"
      end
    end
  end

  # Api.hh
  def print_cxx_api_head
    for_each_fun do |fn|
      if not fn.dumps then
        @f.print cxx_proto(fn, "", "  ");
        @f.print ";"
      end
    end
  end

  # Constant.hh
  def print_cst
    build_constants
    build_enums
    build_structs
  end
end

def do_nothing
end

gen = CxxFileGenerator.new
files.each do |fn|
  gen.expand_variables(fn)
end
gen.fill_file_section("Api.cc") { gen.print_cxx_api }
gen.fill_file_section("Api.hh") { gen.print_cxx_api_head }
gen.fill_file_section("interface.cc") { gen.print_interface }
gen.fill_file_section("Constant.hh") { gen.print_cst }

puts "Done."
