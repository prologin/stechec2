import unittest

from generator.game import GameError
from generator.test.utils import load_game


class TestYamlErrors(unittest.TestCase):
    def test_error_multi_args(self):
        expected = ("Function 'foobar_fail': Argument 'barfoo' was defined "
                    "twice")
        with self.assertRaisesRegex(GameError, expected):
            load_game('test-fail-multi-args')

    def test_error_arg_type(self):
        expected = ("Function 'foobar_fail': Argument 'sometype' conflicts "
                    "with a type name")
        with self.assertRaisesRegex(GameError, expected):
            load_game('test-fail-arg-type')

    def test_error_unknown_type(self):
        expected = ("Function 'foobar_fail': Argument 'foobar' of unknown "
                    "type 'whatever'")
        with self.assertRaisesRegex(GameError, expected):
            load_game('test-fail-unknown-type')
