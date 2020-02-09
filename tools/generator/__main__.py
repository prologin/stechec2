#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2020 Association Prologin <association@prologin.org>

import argparse
import subprocess
import textwrap
import yaml
from pathlib import Path

from .champions import make_champions
from .rules import make_rules
from .texdoc import make_texdoc
from .sphinxdoc import make_sphinxdoc


def main():
    parser = argparse.ArgumentParser(
        description=textwrap.dedent(
            """\
            stechec2-generator is a code generation tool which generates the
            rules boilerplate, player code stubs and makefiles for a game based
            on its YAML description file.
            """))

    sp = parser.add_subparsers(dest='command', help='Generator to use')
    sp.add_parser('player', help="create player stubs for all languages")
    sp.add_parser('champions', help="create player stubs for all languages (experimental)")
    sp.add_parser('server', help="create special Makefiles for the workernode")
    sp.add_parser('rules', help="generate boilerplate for api rules")
    sp.add_parser('texdoc', help="generate latex API doc of the game")
    sp.add_parser('sphinxdoc', help="generate sphinx API doc of the game")

    sp.add_parser('ruby-player')
    sp.add_parser('ruby-server')
    sp.add_parser('ruby-rules')
    sp.add_parser('ruby-apidoc')
    sp.add_parser('ruby-sphinxdoc')

    parser.add_argument('yaml_file', type=argparse.FileType('r'),
                        help="The game YAML file")
    parser.add_argument('out_dir', type=Path, help="The output directory")
    args = parser.parse_args()

    # TODO check that conf is valid
    game = yaml.safe_load(args.yaml_file)

    args.out_dir.mkdir(parents=True, exist_ok=True)

    if args.command == 'rules':
        make_rules(game, args.out_dir)
    elif args.command == 'champions':
        make_champions(game, args.out_dir)
    elif args.command == 'texdoc':
        make_texdoc(game, args.out_dir)
    elif args.command == 'sphinxdoc':
        make_sphinxdoc(game, args.out_dir)
    else:
        command = args.command
        if command.startswith('ruby-'):
            command = command[len('ruby-'):]
        subprocess.run(
            ['stechec2-ruby-generator', command, args.yaml_file.name,
             str(args.out_dir)],
            check=True
        )


if __name__ == "__main__":
    main()
