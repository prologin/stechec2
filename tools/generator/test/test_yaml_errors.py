import unittest

from generator.game import Game, GameError


def get_test_game(content):
    g = {
        'name': 'test',
        'constant': [],
        'enum': [],
        'struct': [],
        'function': [],
        'user_function': [],
    }
    g.update(content)
    return g


class TestYamlErrors(unittest.TestCase):
    def test_error_func_arg_redefined(self):
        expected = ("Function 'foobar_fail': Argument 'barfoo' was defined "
                    "twice")
        with self.assertRaisesRegex(GameError, expected):
            Game(get_test_game({
                'function': [{
                    'fct_name': 'foobar_fail',
                    'fct_summary': 'same argument is here twice',
                    'fct_ret_type': 'int',
                    'fct_arg': [
                        ['foobar', 'int', ''],
                        ['barfoo', 'int', ''],
                        ['barfoo', 'string', ''],
                    ],
                }]
            }))

    def test_error_struct_shadows_enum(self):
        expected = "Type sometype was defined twice."
        with self.assertRaisesRegex(GameError, expected):
            Game(get_test_game({
                'enum': [{
                    'enum_name': 'sometype',
                    'enum_summary': 'some type',
                    'enum_field': [
                        ['x', 'some field']
                    ]
                }],
                'struct': [{
                    'str_name': 'sometype',
                    'str_summary': 'some type',
                    'str_field': [
                        ['y', 'int', 'some field']
                    ]
                }],
            }))

    def test_error_struct_field_typename(self):
        expected = ('Name "conflict" conflicts: a field of struct '
                    '"structtype", an enum name')
        with self.assertRaisesRegex(GameError, expected):
            Game(get_test_game({
                'enum': [{
                    'enum_name': 'conflict',
                    'enum_summary': 'enum type',
                    'enum_field': [
                        ['x', 'some field']
                    ]
                }],
                'struct': [{
                    'str_name': 'structtype',
                    'str_summary': 'struct type',
                    'str_field': [
                        ['conflict', 'int', 'field with conflict name']
                    ]
                }],
            }))

    def test_error_func_arg_type_conflict(self):
        expected = ("Function 'foobar_fail': Argument 'sometype' conflicts "
                    "with a type name")
        with self.assertRaisesRegex(GameError, expected):
            Game(get_test_game({
                'struct': [{
                    'str_name': 'sometype',
                    'str_summary': 'some type',
                    'str_field': [
                        ['x', 'int', 'some field']
                    ]
                }],
                'function': [{
                    'fct_name': 'foobar_fail',
                    'fct_summary': 'argument and type have same name',
                    'fct_ret_type': 'int',
                    'fct_arg': [
                        ['foo', 'sometype', ''],
                        ['sometype', 'int', ''],
                    ],
                }]
            }))

    def test_error_func_unknown_arg_type(self):
        expected = ("Function 'foobar_fail': Argument 'foobar' of unknown "
                    "type 'whatever'")
        with self.assertRaisesRegex(GameError, expected):
            Game(get_test_game({
                'function': [{
                    'fct_name': 'foobar_fail',
                    'fct_summary': 'argument type is unknown',
                    'fct_ret_type': 'int',
                    'fct_arg': [
                        ['foobar', 'whatever', ''],
                    ],
                }]
            }))

    def test_error_func_unknown_return_type(self):
        expected = ("Function 'foobar_fail' has an unknown return type "
                    "'whatever'")
        with self.assertRaisesRegex(GameError, expected):
            Game(get_test_game({
                'function': [{
                    'fct_name': 'foobar_fail',
                    'fct_summary': 'argument type is unknown',
                    'fct_ret_type': 'whatever',
                    'fct_arg': [],
                }]
            }))

    def test_error_func_defined_twice(self):
        expected = "Function 'foobar_fail' was defined twice."
        with self.assertRaisesRegex(GameError, expected):
            Game(get_test_game({
                'function': [
                    {
                        'fct_name': 'foobar_fail',
                        'fct_summary': 'defined once',
                        'fct_ret_type': 'int',
                        'fct_arg': [],
                    },
                    {
                        'fct_name': 'foobar_fail',
                        'fct_summary': 'defined twice',
                        'fct_ret_type': 'double',
                        'fct_arg': [],
                    },
                ]
            }))

    def test_error_unicity_struct_fields(self):
        expected = ('Name "conflict" conflicts: a field of struct '
                    '"structtype", a field of enum "enumtype')
        with self.assertRaisesRegex(GameError, expected):
            Game(get_test_game({
                'enum': [{
                    'enum_name': 'enumtype',
                    'enum_summary': 'enum type',
                    'enum_field': [
                        ['conflict', 'some field']
                    ]
                }],
                'struct': [{
                    'str_name': 'structtype',
                    'str_summary': 'struct type',
                    'str_field': [
                        ['conflict', 'int', 'field with conflict name']
                    ]
                }],
            }))

    def test_error_name_reserved_keyword(self):
        expected = ("Game name 'bool': name conflicts with a keyword of a "
                    "target language.")
        with self.assertRaisesRegex(GameError, expected):
            Game(get_test_game({
                'name': 'bool'
            }))

    def test_error_function_reserved_keyword(self):
        expected = ("Function 'static': name conflicts with a keyword of a "
                    "target language.")
        with self.assertRaisesRegex(GameError, expected):
            Game(get_test_game({
                'function': [{
                    'fct_name': 'static',
                    'fct_summary': 'static is a keyword',
                    'fct_ret_type': 'int',
                    'fct_arg': [],
                }]
            }))

    def test_error_function_arg_reserved_keyword(self):
        expected = ("Function add_stuff: argument 'type': name conflicts with "
                    "a keyword of a target language.")
        with self.assertRaisesRegex(GameError, expected):
            Game(get_test_game({
                'enum': [{
                    'enum_name': 'error',
                    'enum_summary': 'error',
                    'enum_field': [
                        ['ok', 'OK']
                    ]
                }],
                'function': [{
                    'fct_name': 'add_stuff',
                    'fct_summary': 'add a thing',
                    'fct_ret_type': 'error',
                    'fct_action': 'yes',
                    'fct_arg': [['type', 'int', 'Type of stuff to add']],
                }]
            }))
