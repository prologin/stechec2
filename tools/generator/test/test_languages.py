# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2020 Association Prologin <association@prologin.org>
# Copyright (c) 2020 Antoine Pietri

import contextlib
import os
import pathlib
import shutil
import subprocess
import unittest
from sys import stderr

from generator.game import load_game
from generator.player import check_player, check_compile
from generator.test.utils import generate_player, load_test_game


class TestLanguages(unittest.TestCase):
    def setUp(self):
        with contextlib.ExitStack() as stack:
            tmp = generate_player(load_test_game('test'))
            self.player_path = stack.enter_context(tmp)
            self.addCleanup(stack.pop_all().close)

    def test_c(self):
        self.run_language_tests('c', 'champion.c')

    def test_caml(self):
        self.run_language_tests('caml', 'champion.ml')

    def test_csharp(self):
        self.run_language_tests('cs', 'champion.cs')

    def test_cxx(self):
        self.run_language_tests('cxx', 'champion.cc')

    def test_haskell(self):
        self.run_language_tests('haskell', 'Champion.hs')

    def test_java(self):
        self.run_language_tests('java', 'Champion.java')

    def test_php(self):
        self.run_language_tests('php', 'champion.php')

    def test_python(self):
        self.run_language_tests('python', 'Champion.py')

    def test_rust(self):
        self.run_language_tests('rust', 'champion.rs')

    def run_language_tests(self, language, champion_file_name):
        language_dir = self.player_path / language
        self.compile_language(language, champion_file_name)
        self.check_language(language_dir)

    def check_language(self, language_dir):
        env = os.environ.copy()
        env['LD_LIBRARY_PATH'] = '.:{}'.format(env.get('LD_LIBRARY_PATH', ''))

        msg_assert = "test_alert() did not assert, assertions are not working."
        with self.assertRaises(subprocess.CalledProcessError, msg=msg_assert):
            subprocess.run(
                ['./tester', '--test-alert'], universal_newlines=True,
                check=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                cwd=language_dir, env=env
            )

        try:
            subprocess.run(
                ['./tester'], universal_newlines=True, check=True,
                stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                cwd=language_dir, env=env
            )
        except subprocess.CalledProcessError as e:
            self.fail("Test failed with output:\n" + e.output)

    def compile_language(self, language, champion_file_name):
        language_dir = self.player_path / language
        language_test_dir = pathlib.Path(__file__).parent / 'languages'

        # Copy champion file
        shutil.copy2(language_test_dir / champion_file_name, language_dir)

        # Compile champion.so
        err = check_compile(language_dir)
        if err:
            self.fail("Champion compilation failed with output:\n" + err)

        # Compile tester
        shutil.copy2(language_test_dir / 'tester.cc', self.player_path)
        try:
            subprocess.run(
                [
                    'g++', '-std=c++17', '-g', '-Wall', '-Wextra', '-Werror',
                    '-o', 'tester', '../tester.cc', './champion.so'
                ],
                universal_newlines=True, check=True,
                stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                cwd=language_dir,
            )
        except subprocess.CalledProcessError as e:
            self.fail("Tester compilation failed with output:\n" + e.output)


class TestExampleGames(unittest.TestCase):
    def test_example_games(self):
        file = pathlib.Path(__file__)
        GAMES_DIR = file.parent.parent.parent.parent / 'games'
        success = True
        with contextlib.ExitStack() as stack:
            for game in filter(pathlib.Path.is_dir, GAMES_DIR.iterdir()):
                config = game / f"{game.name}.yml"
                with config.open() as f:
                    tmp = generate_player(load_game(f))
                    player_path = stack.enter_context(tmp)
                print(f"\n--- {game.name} ---", file=stderr)
                success &= check_player(player_path)

            self.addCleanup(stack.pop_all().close)

        if not success:
            self.fail("some tests failed")
