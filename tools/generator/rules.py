# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2020 Association Prologin <association@prologin.org>

from pathlib import Path

from .generator import Generator


def make_rules(game, out_dir: Path) -> None:
    """Generate the template to code a new stechec rules project"""

    gen = Generator('rules', game=game, out_dir=out_dir)

    base_files = [
        'src/actions.hh',
        'src/api.cc',
        'src/api.hh',
        'src/constant.hh',
        'src/interface.cc',
        'src/rules.hh',
        'src/rules.cc',
        'src/entry.cc',
        'src/game_state.cc',
        'src/game_state.hh',
        'wscript'
    ]
    for tpl in base_files:
        gen.template(tpl)

    for action in game["function"]:
        if not action.get("fct_action"):
            continue
        for ext in ('hh', 'cc'):
            gen.template('src/action_template.' + ext,
                         out_name='src/action_{}.{}'.format(
                             action['fct_name'], ext),
                         action=action)
