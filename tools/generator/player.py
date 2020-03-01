# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2020 Association Prologin <association@prologin.org>

import os
from pathlib import Path

from .generator import Generator


def gen_lang(game, out_dir: Path, lang: str, files) -> None:
    template_dir = os.path.join('player', lang)
    gen = Generator(template_dir, game=game, out_dir=out_dir / lang)

    for tpl in files:
        gen.template(tpl)


def make_player(game, out_dir: Path) -> None:
    """Generate the template to code a champion for a stechec project"""

    gen_lang(game, out_dir, 'c',
             ['api.h', 'champion.c', 'interface.cc', 'Makefile', 'Makefile-c'])
    gen_lang(game, out_dir, 'cxx',
             ['api.hh', 'champion.cc', 'interface.cc', 'Makefile',
              'Makefile-cxx'])
    gen_lang(game, out_dir, 'php',
             ['api.php', 'champion.php', 'interface.cc', 'Makefile',
              'Makefile-php'])
    gen_lang(game, out_dir, 'python',
             ['api.py', 'Champion.py', 'interface.cc', 'Makefile',
              'Makefile-python'])
