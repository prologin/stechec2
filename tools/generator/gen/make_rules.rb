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
install_path = Pathname.new($install_path) + package_name + "rules"
install_path.mkpath

if $conf["rules_type"] == nil then
  $conf["rules_type"] = "none"
end
rules_types = { "none" => nil,
                "turnbased" => "TurnBasedRules",
                "synchronous" => "SynchronousRules" }
unless rules_types.include?($conf["rules_type"])
  abort("Supported rules_type: 'none', 'synchronous' and 'turnbased'.")
end
$conf["rules_type"] = rules_types[$conf["rules_type"]]

# Copie les quelques squelettes.
files = %w{
  api.cc
  api.hh
  constant.hh
  interface.cc
  rules.cc
  rules.hh
  entry.cc
  game_state.cc
  game_state.hh
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
    replaces['!!provider!!'] = 'Prologin' # FIXME
    replaces['!!year!!'] = Date.today.year.to_s
    replaces['!!package_name!!'] = $conf['conf']['package_name']
    if $conf['rules_type'].nil? then
        replaces['!!rules_hh_inheritance!!'] = ""
        replaces['!!rules_cc_inheritance!!'] = ""
    else
        replaces['!!rules_hh_inheritance!!'] = (" : public rules::" +
                                                $conf['rules_type'])
        replaces['!!rules_cc_inheritance!!'] = (" " + $conf['rules_type'] +
                                                "(opt),")
    end
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

  # Cette fonction utilitaire est copiée plein de fois à travers le code
  # TODO: factoriser
  def camel_case(str)
    strs = str.split("_")
    strs.each { |s| s.capitalize! }
    strs.join
  end

  # api.cc
  def print_cxx_api
    for_each_fun(true, "function", false) do |fn|
      if not fn.dumps then
        # genérer plein de boilerplate pour les actions
        if fn.conf['fct_action'] then
          class_name = "Action" + (camel_case fn.name)
          arg_list = fn.args.map(&:name) + ["player_->id"]
          err_type = fn.ret
          unless err_type.is_enum?
            abort("Return type of action #{fn.name} is not an enumeration.")
          end
          err_type_name = err_type.conf['enum_name']
          ok_val = err_type.conf['enum_field'][0][0].upcase

          @f.puts cxx_proto(fn, "Api::")
          @f.puts <<-EOF
{
    rules::IAction_sptr action(new #{class_name}(#{arg_list.join(", ")}));

    #{err_type_name} err;
    if ((err = static_cast<#{err_type_name}>(action->check(game_state_))) != #{ok_val})
        return err;

    actions_.add(action);
    game_state_set(action->apply(game_state_));
    return OK;
}
EOF
        else
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
std::string convert_to_string(double i){
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

  # actions
  def fn_is_action?(fn)
    (not fn.dumps) && fn.conf['fct_action']
  end

  def print_actions_hh
    # I don't think for_each_fun without block returns an enumerator
    # (which it really should)
    # so this code is less nice / more imperative than it could have been
    action_ids = []
    includes = []
    for_each_fun(false) do |fn|
      if (not fn.dumps) && fn.conf['fct_action']
        action_ids << "ID_ACTION_#{fn.name.upcase}"
        includes << "#include \"action_#{fn.name}.hh\""
      end
    end

    File.open("actions.hh", 'w') do |file|
      file.write <<-EOF
#ifndef ACTIONS_HH
#define ACTIONS_HH

enum action_id {
    #{action_ids.join(",\n    ")}
};

#{includes.join("\n")}

#endif // !ACTIONS_HH
EOF
    end
  end

  def print_action_file(fn)
    return nil unless fn_is_action? fn

    puts "Generating action file for #{fn.name}"

    ifdef_name = "ACTION_#{fn.name.upcase}_HH"
    class_name = "Action" + (camel_case fn.name)

    arg_list = fn.args.map { |arg| [cxx_type(arg.type), arg.name] } \
               + [["int", "player_id"]]

    constr_args = arg_list.map{|t,v| "#{t} #{v}"}.join(", ")
    constr_proto = class_name + '(' + constr_args + ')'

    members = arg_list.map{|t,v| "#{t} #{v}_;"}.join("\n    ")
    bufhandle = arg_list.map{|t,v| "buf.handle(#{v}_);"}.join("\n        ")

    init_list = arg_list.map{|t,v| "#{v}_(#{v})"}.join(", ")

    File.open("action_#{fn.name}.hh", 'w') do |file|
      file.write <<-EOF
#ifndef #{ifdef_name}
#define #{ifdef_name}

#include <rules/action.hh>

#include "actions.hh"
#include "game_state.hh"
#include "constant.hh"

class #{class_name} : public rules::Action<GameState>
{
public:
    #{constr_proto} : #{init_list} {}
    #{class_name}() {} // for register_action()

    int check(const GameState* st) const override;
    void apply_on(GameState* st) const override;

    void handle_buffer(utils::Buffer& buf) override
    {
        #{bufhandle}
    }

    uint32_t player_id() const override { return player_id_; };
    uint32_t id() const override { return ID_ACTION_#{fn.name.upcase}; }

private:
    #{members}
};

#endif // !#{ifdef_name}
EOF
    end
    File.open("action_#{fn.name}.cc", 'w') do |file|
      file.write <<-EOF
#include "actions.hh"

int #{class_name}::check(const GameState* st) const
{
    // FIXME
    return 0;
}

void #{class_name}::apply_on(GameState* st) const
{
    // FIXME
}
EOF
    end
  end

  # api.hh
  def print_cxx_api_head
    for_each_fun(true, "function", false, "    ") do |fn|
      if not fn.dumps then
        @f.print cxx_proto(fn, "", "   ");
        @f.print ";\n"
      end
    end
  end

  def print_cxx_rules_head
    if $conf["rules_type"] == nil
      @f.write <<-EOF
    void player_loop(rules::ClientMessenger_sptr msgr) override;
    void spectator_loop(rules::ClientMessenger_sptr msgr) override;
    void server_loop(rules::ServerMessenger_sptr msgr) override;
EOF
    else
      @f.write <<-EOF
    rules::Actions* get_actions() override;
    void apply_action(const rules::IAction_sptr& action) override;
    bool is_finished() override;

protected:
    // FIXME: Override #{$conf["rules_type"]} methods here
EOF
    end
  end

  def print_cxx_rules
    # Register actions
    @f.print "void Rules::register_actions()\n{\n"
    for_each_fun(false) do |fn|
      if fn_is_action? fn then
        @f.print <<-EOF
    api_->actions()->register_action(
        ID_ACTION_#{fn.name.upcase},
        []() -> rules::IAction* { return new Action#{camel_case fn.name}(); }
        );
EOF
      end
    end
    @f.print "}\n\n"

    # Rules boilerplate
    if $conf["rules_type"] == nil
      @f.write <<-EOF
void Rules::player_loop(rules::ClientMessenger_sptr msgr)
{
    // FIXME
}

void Rules::spectator_loop(rules::ClientMessenger_sptr msgr)
{
    // FIXME
}

void Rules::server_loop(rules::ServerMessenger_sptr msgr)
{
    // FIXME
}
EOF
    else
      @f.write <<-EOF
rules::Actions* Rules::get_actions()
{
    return api_->actions();
}

void Rules::apply_action(const rules::IAction_sptr& action)
{
    // When receiving an action, the API should have already checked that it
    // is valid. We recheck that for the current gamestate here to avoid weird
    // client/server desynchronizations and make sure the gamestate is always
    // consistent across the clients and the server.

    int err = action->check(api_->game_state());
    if (err)
        FATAL("Synchronization error: received action %d from player %d, but "
              "check() on current gamestate returned %d.",
              action->id(), action->player_id(), err);
    api_->game_state_set(action->apply(api_->game_state()));
}

bool Rules::is_finished()
{
    // FIXME
    return true;
}
EOF
    end
  end


  # Constant.hh
  def print_cst
    build_constants
    build_enums
    build_structs
  end
end

gen = CxxFileGenerator.new
files.each do |fn|
  gen.expand_variables(fn)
end
gen.fill_file_section("api.cc") { gen.print_cxx_api }
gen.fill_file_section("api.hh") { gen.print_cxx_api_head }
gen.fill_file_section("interface.cc") { gen.print_interface }
gen.fill_file_section("constant.hh") { gen.print_cst }
gen.fill_file_section("rules.cc") { gen.print_cxx_rules }
gen.fill_file_section("rules.hh") { gen.print_cxx_rules_head }
gen.print_actions_hh
gen.for_each_fun(false) do |fn|
  gen.print_action_file(fn)
end

puts "Done."
