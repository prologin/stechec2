#!/usr/bin/env python3

import argparse
import subprocess
import textwrap
import yaml
from pathlib import Path

from .rules import make_rules
from .texdoc import make_texdoc


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
    sp.add_parser('server', help="create special Makefiles for the workernode")
    sp.add_parser('rules', help="generate boilerplate for api rules")
    sp.add_parser('apidoc', help="generate latex API doc of the game")
    sp.add_parser('sphinxdoc', help="generate sphinx API doc of the game")

    parser.add_argument('yaml_file', help="The game YAML file")
    parser.add_argument('out_dir', help="The output directory")
    args = parser.parse_args()

    # TODO check that conf is valid
    game = yaml.safe_load(open(args.yaml_file, "r"))

    Path(args.out_dir).mkdir(parents=True, exist_ok=True)

    if args.command == 'rules':
        make_rules(game, args.out_dir)
    elif args.command == 'apidoc':
        make_texdoc(game, args.out_dir)
    else:
        subprocess.run(
            ['stechec2-ruby-generator', args.command, args.yaml_file,
             args.out_dir],
            check=True
        )


if __name__ == "__main__":
    main()
