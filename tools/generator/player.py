# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2020 Association Prologin <association@prologin.org>
# Copyright (c) 2020 Sacha Delanoue
# Copyright (c) 2020 Antoine Pietri

import os
import subprocess
from pathlib import Path
from sys import stderr
from typing import Optional

from .generator import Generator


LANGUAGES = {
    'c': {
        'files': ['champion.c', 'Makefile'],
        'symlinks': ['api.h', 'interface.cc', 'Makefile-c']},
    'caml': {
        'files': ['champion.ml', 'champion.mli', 'Makefile'],
        'symlinks': ['api.ml', 'interface.cc', 'Makefile-caml']},
    'cs': {
        'files': ['champion.cs', 'api.cs', 'interface.cc', 'Makefile'],
        'symlinks': ['Makefile-cs']},
    'cxx': {
        'files': ['champion.cc', 'Makefile'],
        'symlinks': ['api.hh', 'interface.cc', 'Makefile-cxx']},
    'haskell': {
        'files': ['Champion.hs', 'Makefile'],
        'symlinks': ['Api.hs', 'CApi.hsc', 'api.h', 'interface.cc',
                     'Makefile-haskell']},
    'java': {
        'files': ['Champion.java', 'Makefile'],
        'symlinks': ['Api.java', 'interface.cc', 'Makefile-java']},
    'php': {
        'files': ['champion.php', 'Makefile'],
        'symlinks': ['api.php', 'interface.cc', 'Makefile-php']},
    'python': {
        'files': ['Champion.py', 'Makefile'],
        'symlinks': ['api.py', 'interface.cc', 'Makefile-python']},
    'rust': {
        'files': ['Makefile', 'champion.rs'],
        'symlinks': ['Cargo.toml', 'Makefile-rust', 'api.rs', 'ffi.rs',
                     'api.h', 'interface.cc']},
}


def make_player(game, out_dir: Path, symlink: Path = None) -> None:
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
                new_path.symlink_to(source_path.resolve())
            else:
                gen.template(name)

    for lang, files in LANGUAGES.items():
        gen_lang(lang, **files)


def check_compile(lang_dir: Path) -> Optional[str]:
    """Try to `make` a player, returns `None` on success
    or the error message in case of a failure"""

    try:
        subprocess.run(
            ['make'], universal_newlines=True, check=True,
            capture_output=True, cwd=lang_dir,
        )
    except subprocess.CalledProcessError as e:
        return e.stderr


def check_tar(lang_dir: Path) -> Optional[str]:
    """Try to `make tar` a player, returns `None` on success
    or the error message in case of a failure"""
    try:
        subprocess.run(
            ['make', 'tar'], universal_newlines=True, check=True,
            stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
            cwd=lang_dir,
        )
    except subprocess.CalledProcessError as e:
        return e.stderr


def check_player(player_dir: Path) -> bool:
    """Run `make` for all languages for an already generated player"""

    # Print on stderr
    def eprint(*args, **kwargs):
        print(*args, file=stderr, **kwargs)

    success = True
    for lang in LANGUAGES.keys():
        eprint(f"{lang}...", end='', flush=True)
        err = check_compile(player_dir / lang) or check_tar(player_dir / lang)
        if err is None:
            eprint("OK")
        else:
            eprint("FAILED")
            eprint(err)
            success = False
    return success
