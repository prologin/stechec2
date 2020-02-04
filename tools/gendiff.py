#!/usr/bin/env python3

# Copyright (c) 2020 Antoine Pietri <antoine.pietri@prologin.org>

"""
gendiff.py is a very useful tool to see the impact of your changes in the
generators. It works by generating the output files of a test game using
two different versions of the generators, and diffing the outputs.

Here are a few usage examples:

(1) If you just changed the rules generator and want to see how the output of
the generator would change:

    tools/gendiff.py rules

(2) If you want to see how the sphinxdoc generator output changed between your
current working tree and a previous commit:

    tools/gendiff.py --from HEAD~2 sphinxdoc

(3) If you want to check the output differences between two commits:

    tools/gendiff.py --from 1b87599 --to HEAD~1 sphinxdoc

(4) You can also diff between different *commands* (as opposed to *versions*),
which is particularly useful to test full rewrites of the generators:

    tools/gendiff.py --from-command=ruby-rules rules

(5) All unknown options are passed directly to git-diff(1), so e.g you can:

    # ignore all whitespace changes
    tools/gendiff.py -w rules

    # do a color word-diff:
    tools/gendiff.py rules --word-diff=color

    # only look at modified files (= filter out additions and removals):
    tools/gendiff.py rules --diff-filter=M
"""

import argparse
import contextlib
import os
import os.path
import subprocess
import tempfile
from pathlib import Path

REPO_PATH = Path(__file__).parent.parent
DEFAULT_GAME_YML = REPO_PATH / 'games/tictactoe/tictactoe.yml'


@contextlib.contextmanager
def generate(base_path, command, game_config):
    generator_path = Path(base_path) / 'tools'
    with tempfile.TemporaryDirectory(prefix='stechec2-gendiff-gen-') as td:
        td = Path(td)
        subprocess.run(
            ['python3', '-m', 'generator', command, game_config, str(td)],
            env={**os.environ, 'PYTHONPATH': generator_path}
        )
        # Specialcase legacy ruby behavior to help diff track renames
        if command == 'ruby-rules':
            rules = list(td.glob('*/rules'))
            if rules:
                rules[0].rename(td / 'src')
        yield td


@contextlib.contextmanager
def git_checkout(commit):
    with tempfile.TemporaryDirectory(prefix='stechec2-gendiff-rev-') as td:
        subprocess.run(
            ['git', '--work-tree', td, 'checkout', commit, '--', 'tools']
        )
        yield td


def main():
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawTextHelpFormatter,
        description=(
            "Show the diff between the generated output of different versions "
            "of the generators.\n\n"
            + __doc__
        )
    )
    parser.add_argument(
        '-g', '--game',
        default=DEFAULT_GAME_YML,
        help="Use this game config file."
    )
    parser.add_argument(
        '--from', default='HEAD', dest='from_',
        help="Diff using this commit as the source."
    )
    parser.add_argument(
        '--from-command',
        help="Run this command on the 'from' commit instead of 'command'."
    )
    parser.add_argument(
        '--to',
        help=("Diff against this destination commit. "
              "Defaults to current working tree.")
    )
    parser.add_argument('command', help="Generator command to diff")
    args, diff_args = parser.parse_known_args()

    with contextlib.ExitStack() as stack:
        path_from = stack.enter_context(git_checkout(args.from_))
        if args.to:
            path_to = stack.enter_context(git_checkout(args.to))
        else:
            path_to = REPO_PATH

        if args.from_command:
            from_command = args.from_command
        else:
            from_command = args.command
        gen_from = stack.enter_context(
            generate(path_from, from_command, args.game))
        gen_to = stack.enter_context(
            generate(path_to, args.command, args.game))

        subprocess.run(
            ['git', 'diff', '--find-renames', '--no-index', *diff_args,
             str(gen_from), str(gen_to)]
        )


if __name__ == '__main__':
    main()
