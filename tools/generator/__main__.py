#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2020 Association Prologin <association@prologin.org>

import argparse
import textwrap
from pathlib import Path

from .game import load_game
from .player import make_player
from .rules import make_rules
from .texdoc import make_texdoc
from .sphinxdoc import make_sphinxdoc


def get_install_prefix():
    """
    Check if the generator is being run from a stechec2 install prefix and
    return the prefix path.
    """
    path = Path(__file__).parent.parent.parent.parent
    if (path / 'bin/stechec2-generator').exists():
        return path
    else:
        return None


def game_or_yaml_path(path):
    """
    argparse.FileType that reads either from the given path, or from a
    matching game config file in the current stechec2 install prefix.
    """
    prefix = get_install_prefix()
    if prefix:
        game_path = prefix / 'share/stechec2/{f}/{f}.yml'.format(f=path)
        if game_path.exists():
            path = game_path
    return argparse.FileType('r')(path)


def main():
    parser = argparse.ArgumentParser(
        description=textwrap.dedent(
            """\
            stechec2-generator is a code generation tool which generates the
            rules boilerplate, player code stubs and makefiles for a game based
            on its YAML description file.
            """))
    sp = parser.add_subparsers(dest='command', help='Generator to use')

    player_subparser = sp.add_parser(
        'player', help="create player stubs for all languages"
    )
    player_subparser.add_argument(
        '--symlink', nargs='?', type=Path, const=True,
        help=("Instead of generating the read-only API and interface files, "
              "symlink them from a pre-generated player environment. "
              "This avoids regenerating all the environments when the API "
              "changes. If no path is given, defaults to the default rules "
              "install path.")
    )
    player_subparser.add_argument(
        '--symlink-from', type=Path,
        help=("create symlinked player environment (conflicts with --symlink)"))
    player_subparser.add_argument(
        '--no-resolve', action='store_true', default=False)

    sp.add_parser('rules', help="generate boilerplate for api rules")
    sp.add_parser('texdoc', help="generate latex API doc of the game")
    sp.add_parser('sphinxdoc', help="generate sphinx API doc of the game")

    parser.add_argument('yaml_file', type=game_or_yaml_path,
                        help="The game YAML file")
    parser.add_argument('out_dir', type=Path, help="The output directory")
    args = parser.parse_args()

    game = load_game(args.yaml_file)
    args.out_dir.mkdir(parents=True, exist_ok=True)

    if args.command == 'rules':
        make_rules(game, args.out_dir)
    elif args.command == 'player':
        symlink = None
        if args.symlink and args.symlink_from:
            raise RuntimeError(
                 "--symlink and --symlink-from are mutually exclusive")

        if args.symlink_from:
            symlink = args.symlink_from

        if args.symlink:
            if args.symlink is True:  # Use default install path
                prefix = get_install_prefix()
                if not prefix:
                    raise RuntimeError(
                        "Cannot use --symlink without a path from a "
                        "non-installed generator"
                    )
                symlink = prefix / ('share/stechec2/{}/player'
                                    .format(game['name']))
            else:
                symlink = args.symlink
        make_player(game, args.out_dir, symlink=symlink, resolve=not args.no_resolve)
    elif args.command == 'texdoc':
        make_texdoc(game, args.out_dir)
    elif args.command == 'sphinxdoc':
        make_sphinxdoc(game, args.out_dir)
    else:
        raise RuntimeError("Unknown command {}.".format(args.command))


if __name__ == "__main__":
    main()
