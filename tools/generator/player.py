# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2020 Association Prologin <association@prologin.org>
# Copyright (c) 2020 Sacha Delanoue
# Copyright (c) 2020 Antoine Pietri

import os
from pathlib import Path

from .generator import Generator


def make_player(game, out_dir: Path, symlink: Path = None, resolve: bool = True) -> None:
    """Generate the template to code a champion for a stechec project"""

    def gen_lang(lang: str, *, files, symlinks) -> None:
        template_dir = os.path.join('player', lang)
        gen = Generator(template_dir, game=game, out_dir=out_dir / lang)
        for name in files:
            gen.template(name)
        for name in symlinks:
            if symlink:
                source_path = symlink / lang / name
                if not source_path.exists():
                    raise RuntimeError("Symlink source file not found: {}"
                                       .format(source_path))
                new_path = (gen.out_dir / name)
                try:
                    new_path.unlink()
                except FileNotFoundError:
                    pass
                new_path.symlink_to(source_path.resolve() if resolve else source_path)
            else:
                gen.template(name)

    gen_lang('c',
             files=['champion.c', 'Makefile'],
             symlinks=['api.h', 'interface.cc', 'Makefile-c'])
    gen_lang('caml',
             files=['champion.ml', 'champion.mli', 'Makefile'],
             symlinks=['api.ml', 'interface.cc', 'Makefile-caml'])
    gen_lang('cs',
             files=['champion.cs', 'api.cs', 'interface.cc', 'Makefile'],
             symlinks=['Makefile-cs'])
    gen_lang('cxx',
             files=['champion.cc', 'Makefile'],
             symlinks=['api.hh', 'interface.cc', 'Makefile-cxx'])
    gen_lang('haskell',
             files=['Champion.hs', 'Makefile'],
             symlinks=['Api.hs', 'CApi.hsc', 'api.h', 'interface.cc',
                       'Makefile-haskell'])
    gen_lang('java',
             files=['Champion.java', 'Makefile'],
             symlinks=['Api.java', 'interface.cc', 'Makefile-java'])
    gen_lang('php',
             files=['champion.php', 'Makefile'],
             symlinks=['api.php', 'interface.cc', 'Makefile-php'])
    gen_lang('python',
             files=['Champion.py', 'Makefile'],
             symlinks=['api.py', 'interface.cc', 'Makefile-python'])
    gen_lang('rust',
             files=['Makefile', 'champion.rs'],
             symlinks=['Cargo.toml', 'Makefile-rust', 'api.rs', 'ffi.rs',
                       'api.h', 'interface.cc'])
