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

    def test_cxx(self):
        self.run_language_tests('cxx', 'champion.cc')

    def run_language_tests(self, language, champion_file_name):
        language_dir = self.player_path / language
        self.compile_language(language, champion_file_name)

        try:
            subprocess.run(
                ['./tester'], text=True, check=True,
                stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                cwd=language_dir,
            )
        except subprocess.CalledProcessError as e:
            print(language_dir)
            input()
            self.fail("Test failed with output:\n" + e.output)

    def compile_language(self, language, champion_file_name):
        language_dir = self.player_path / language
        language_test_dir = pathlib.Path(__file__).parent / 'languages'

        # Copy champion file
        shutil.copy2(language_test_dir / champion_file_name, language_dir)

        # Compile champion.so
        try:
            subprocess.run(
                ['make'], text=True, check=True,
                stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                cwd=language_dir,
            )
        except subprocess.CalledProcessError as e:
            print(language_dir)
            input()
            self.fail("Champion compilation failed with output:\n" + e.output)

        # Compile tester
        shutil.copy2(language_test_dir / 'tester.cc', self.player_path)
        try:
            subprocess.run(
                [
                    'g++', '-std=c++17', '-g', '-Wall', '-Wextra', '-Werror',
                    '-o', 'tester', './champion.so', '../tester.cc'
                ],
                text=True, check=True,
                stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                cwd=language_dir,
            )
        except subprocess.CalledProcessError as e:
            self.fail("Tester compilation failed with output:\n" + e.output)
