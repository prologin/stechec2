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

(5) All unknown options are passed directly to git-diff(1), so e.g you can
ignore all whitespace changes by doing:

    tools/gendiff.py -w rules
"""

import argparse
import contextlib
import os
import os.path
import subprocess
import tempfile
from pathlib import Path

REPO_PATH = Path(__file__).parent.parent
GAME_YML_PATH = Path(__file__).parent.parent / 'games/tictactoe/tictactoe.yml'


@contextlib.contextmanager
def generate(base_path, command):
    generator_path = Path(base_path) / 'tools'
    with tempfile.TemporaryDirectory(prefix='stechec2-gendiff-gen-') as td:
        td = Path(td)
        subprocess.run(
            ['python3', '-m', 'generator', command, GAME_YML_PATH, str(td)],
            env={**os.environ, 'PYTHONPATH': generator_path}
        )
        # Specialcase legacy ruby behavior to help diff track renames
        if command == 'ruby-rules':
            (td / 'tictactoe/rules').rename(td / 'src')
        yield td


@contextlib.contextmanager
def git_checkout(commit):
    with tempfile.TemporaryDirectory(prefix='stechec2-gendiff-rev-') as td:
        subprocess.run(
            ['git', '--work-tree', td, 'checkout', commit, '--', 'tools']
        )
        yield td


def main():
    parser = argparse.ArgumentParser(description=(
        "Show the diff between the generated output of different versions of "
        "the generators."
    ))
    parser.add_argument(
        '--from', default='HEAD', dest='from_',
        help="Compare from this commit."
    )
    parser.add_argument(
        '--from-command',
        help="Use this command for the commit compared from instead."
    )
    parser.add_argument(
        '--to',
        help="Compare against this commit. Defaults to current working tree."
    )
    parser.add_argument(
        'command',
        help="Generator command to diff"
    )
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
        gen_from = stack.enter_context(generate(path_from, from_command))
        gen_to = stack.enter_context(generate(path_to, args.command))

        subprocess.run(
            ['git', 'diff', '--find-renames', '--no-index', *diff_args,
             gen_from, gen_to]
        )


if __name__ == '__main__':
    main()
