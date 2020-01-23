import jinja2
import os

from filters.common import camel_case
from filters.cxx import cxx_type, cxx_args, cxx_comment


def make_rules(game, out_dir: str) -> None:
    """Generate the template to code a new stechec rules project"""

    template_folder = os.path.join(os.path.dirname(__file__),
                                   'templates', 'rules')

    env = jinja2.Environment(loader=jinja2.FileSystemLoader(
        searchpath=template_folder))
    env.filters["cxx_type"] = cxx_type
    env.filters["cxx_args"] = cxx_args
    env.filters["camel_case"] = camel_case
    env.filters["cxx_comment"] = cxx_comment

    for gen in ["actions.hh", "api.cc", "api.hh", "constant.hh",
                "interface.cc", "rules.hh", "rules.cc", "entry.cc",
                "game_state.cc", "game_state.hh"]:
        env.get_template(gen + ".jinja2").stream(game=game).dump(os.path.join(
            out_dir, gen))

    for action in game["function"]:
        if "fct_action" in action and action["fct_action"]:
            (env.get_template("action_template.hh.jinja2")
             .stream(action=action)
             .dump(os.path.join(out_dir, "action_")
                   + action["fct_name"] + ".hh"))
            (env.get_template("action_template.cc.jinja2")
             .stream(action=action).dump(os.path.join(out_dir, "action_")
                                         + action["fct_name"] + ".cc"))
