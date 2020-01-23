from .generator import Generator


def make_rules(game, out_dir: str) -> None:
    """Generate the template to code a new stechec rules project"""

    gen = Generator('rules', game=game, out_dir=out_dir)

    base_files = [
        'actions.hh',
        'api.cc',
        'api.hh',
        'constant.hh',
        'interface.cc',
        'rules.hh',
        'rules.cc',
        'entry.cc',
        'game_state.cc',
        'game_state.hh'
    ]
    for tpl in base_files:
        gen.template(tpl)

    for action in game["function"]:
        if not action.get("fct_action"):
            continue
        gen.template('action_template.hh',
                     out_name='action_{}.hh'.format(action['fct_name']),
                     action=action)
        gen.template('action_template.cc',
                     out_name='action_{}.cc'.format(action['fct_name']),
                     action=action)
