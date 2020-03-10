# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2020 Association Prologin <association@prologin.org>
# Copyright (c) 2020 Antoine Pietri

import contextlib
import pathlib
import shutil
import subprocess
import unittest

from generator.test.utils import generate_player


class TestLanguages(unittest.TestCase):
    def setUp(self):
        with contextlib.ExitStack() as stack:
            self.player_path = stack.enter_context(generate_player('test'))
            self.addCleanup(stack.pop_all().close)

    def test_c(self):
        self.run_language_tests('c', 'champion.c')

    def test_caml(self):
        self.run_language_tests('caml', 'champion.ml')

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
        self.check_language_alert(language_dir)
        self.check_language(language_dir)

    def check_language(self, language_dir):
        try:
            subprocess.run(
                ['./tester'], universal_newlines=True, check=True,
                stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                cwd=language_dir,
            )
        except subprocess.CalledProcessError as e:
            self.fail("Test failed with output:\n" + e.output)

    def check_language_alert(self, language_dir):
        msg_assert = "test_alert() did not assert, assertions are not working."
        with self.assertRaises(subprocess.CalledProcessError, msg=msg_assert):
            subprocess.run(
                ['./tester', '--test-alert'], universal_newlines=True,
                check=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                cwd=language_dir,
            )

    def compile_language(self, language, champion_file_name):
        language_dir = self.player_path / language
        language_test_dir = pathlib.Path(__file__).parent / 'languages'

        # Copy champion file
        shutil.copy2(language_test_dir / champion_file_name, language_dir)

        # Compile champion.so
        try:
            subprocess.run(
                ['make'], universal_newlines=True, check=True,
                stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                cwd=language_dir,
            )
        except subprocess.CalledProcessError as e:
            self.fail("Champion compilation failed with output:\n" + e.output)

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
