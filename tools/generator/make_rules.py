#!/usr/bin/env python3

import jinja2
import os
import sys
import yaml
from pathlib import Path
from shutil import copyfile


def cxx_type(value: str) -> str:
    if value == "string":
        return "std::string"
    if value.endswith(" array"):
        return "std::vector<{}>".format(cxx_type(value[:-6]))
    return value

def cxx_args(value) -> str:
    return ", ".join("{} {}".format(cxx_type(i[1]), i[0]) for i in value)

def camel_case(value: str) -> str:
    """Convert a snake case identifier to a upper camel case one"""
    return "".join(i.capitalize() for i in value.split("_"))

@jinja2.environmentfilter
def cxx_comment(env, value: str, doc: bool, indent: int = 0) -> str:
    start = "/// " if doc else "// "
    newline = "\n" + indent * " " + start
    return env.call_filter("wordwrap", start + value, [79, False, start])


def make_rules(yaml_path: str, install_path: str) -> None:
    """Generate the template to code a new stechec rules project"""

    conf = yaml.safe_load(open(yaml_path, "r"))
    # TODO check that conf is valid

    Path(install_path).mkdir(parents=True, exist_ok=True)

    template_folder = "files" # TODO search in PREFIX/share/stechec2 as well

    env = jinja2.Environment(loader=jinja2.FileSystemLoader(searchpath=template_folder))
    env.filters["cxx_type"] = cxx_type
    env.filters["cxx_args"] = cxx_args
    env.filters["camel_case"] = camel_case
    env.filters["cxx_comment"] = cxx_comment

    for copy in ["entry.cc", "game_state.cc", "game_state.hh"]:
        copyfile(os.path.join(template_folder, copy), os.path.join(install_path, copy))

    for gen in ["actions.hh", "api.cc", "api.hh", "constant.hh", "interface.cc", "rules.hh", "rules.cc"]:
        env.get_template(gen + ".jinja2").stream(game=conf).dump(os.path.join(install_path, gen))

    for action in conf["function"]:
        if "fct_action" in action and action["fct_action"]:
            env.get_template("action_template.hh.jinja2").stream(action=action).dump(os.path.join(install_path, "action_") + action["fct_name"] + ".hh")
            env.get_template("action_template.cc.jinja2").stream(action=action).dump(os.path.join(install_path, "action_") + action["fct_name"] + ".cc")

if __name__ == "__main__":
    make_rules(sys.argv[1], sys.argv[2])
