# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2020 Association Prologin <association@prologin.org>

import os
from pathlib import Path

from .generator import Generator


def strip_end(text: str, suffix: str) -> str:
    """Remove given suffix in a string"""
    if not text.endswith(suffix):
        return text
    return text[:len(text) - len(suffix)]


def make_champions(game, out_dir: Path) -> None:
    """Generate the template to code a champion for a stechec project"""

    languages = ['cxx']

    for lang in languages:
        template_dir = os.path.join('champions', lang)
        gen = Generator(template_dir, game=game, out_dir=out_dir / lang)

        for tpl in gen.env.loader.list_templates():
            gen.template(strip_end(tpl, ".jinja2"))
