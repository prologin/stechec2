# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2020-2022 Association Prologin <association@prologin.org>
# Copyright (c) 2020 Antoine Pietri
# Copyright (c) 2020 Rémi Dupré
# Copyright (c) 2020-2022 Sacha Delanoue

import re
import typing
import yaml
from collections import defaultdict
from keyword import iskeyword


def load_game(game_file):
    game = yaml.safe_load(game_file)
    return Game(game)


class GameError(ValueError):
    pass


class Game:
    def __init__(self, game_config):
        self.game = game_config
        self.load()

    def __getitem__(self, name):
        return self.game[name]

    def __len__(self):
        return len(self.game)

    def __iter__(self):
        return iter(self.game)

    def __str__(self):
        return '<Game {}>'.format(self.game['name'])

    def load(self):
        # Define missing fields as empty lists
        self.game['struct'] = self.game.get('struct', [])
        self.game['enum'] = self.game.get('enum', [])
        self.game['function'] = self.game.get('function', [])
        self.game['user_function'] = self.game.get('user_function', [])

        validate_schema(self.game, GAME_SCHEMA)
        self.load_constants()
        self.load_base_types()
        self.load_used_types()
        self.load_funcs()
        self.load_generated_funcs()
        self.check()

    def load_constants(self):
        '''Load the constants and infer their type if necessary.'''
        cst_type_mapping = {
            int: 'int',
            str: 'string',
            float: 'double',
        }
        for cst in self.game['constant']:
            val_type = type(cst['cst_val'])
            if val_type not in cst_type_mapping:
                raise GameError("Constant '{}' has unsupported type: {}"
                                .format(cst['cst_name'], val_type))
            cst_type = cst_type_mapping[val_type]
            if 'cst_type' not in cst:
                cst['cst_type'] = cst_type
            elif cst['cst_type'] != cst_type:
                raise GameError(
                    "Constant '{}' type '{}' does not match value '{}'"
                    .format(cst['cst_name'], cst['cst_type'], cst_type)
                )
            if cst['cst_type'] == 'string':
                # Quotation marks are removed when YAML is parsed,
                # add them back
                cst['cst_val'] = '"{}"'.format(cst['cst_val'])

    def load_base_types(self):
        '''
        Load the builtin types, enum names and struct names defined in the
        game.
        '''
        def add(t):
            if t in self.types:
                raise GameError("Type {} was defined twice.".format(t))
            self.types.add(t)

        self.types = {'void', 'int', 'double', 'bool', 'string'}
        for e in self.game['enum']:
            add(e['enum_name'])
        for s in self.game['struct']:
            add(s['str_name'])

    def load_used_types(self):
        '''
        Load types referenced by struct fields, function arguments and function
        return types in the game objects.
        '''
        def add(t, error):
            if t.endswith(' array'):
                base_type = t[:-len(' array')]
            else:
                base_type = t
            if base_type not in self.types:
                raise GameError(error)
            self.used_types.add(t)

        self.used_types = set()
        for s in self.game['struct']:
            for field_name, t, _ in s['str_field']:
                add(t, ("Struct field '{}.{}' is of unknown type '{}'"
                        .format(s['str_name'], field_name, t)))
        for f in (self.game['function'] + self.game['user_function']):
            add(f['fct_ret_type'],
                ("Function '{}' has an unknown return type '{}'"
                 .format(f['fct_name'], f['fct_ret_type'])))
            for arg_name, t, _ in f['fct_arg']:
                add(t, ("Function '{}': Argument '{}' of unknown type '{}'"
                        .format(f['fct_name'], arg_name, t)))

    def used_array_types(self):
        '''Return the list of types used as arrays in the game objects'''
        return list(sorted({
            t[:-len(' array')] for t in self.used_types if t.endswith(' array')
        }))

    def load_funcs(self):
        defined_funcs = set()  # TODO: store in self
        for f in (self.game['function'] + self.game['user_function']):
            if f['fct_name'] in defined_funcs:
                raise GameError("Function '{}' was defined twice."
                                .format(f['fct_name']))
            defined_funcs.add(f['fct_name'])

    def load_generated_funcs(self):
        def add_display(t):
            self.game['function'].append({
                'fct_name': 'afficher_{}'.format(t),
                'fct_summary': ("Affiche le contenu d'une valeur de type {}"
                                .format(t)),
                'fct_arg': [('v', t, 'The value to display')],
                'fct_ret_type': 'void',
            })
        for e in self.game['enum']:
            add_display(e['enum_name'])
        for s in self.game['struct']:
            add_display(s['str_name'])

    def check(self):
        '''Perform various integrity checks on the game objects'''
        self.check_name_unicity()
        self.check_reserved_keywords()

    def check_name_unicity(self):
        '''
        Check that all the name used are unique.

        Some languages like OCaml infer types from the name of the enum and
        struct fields, and thus require them to be unique *across* different
        types.
        So here we ensure that every name used in the game (except for function
        arguments) are unique.
        '''
        uses = defaultdict(list)
        for basic in ('void', 'int', 'double', 'bool', 'string'):
            uses[basic.lower()].append('a build-in type')
        for constant in self.game['constant']:
            uses[constant['cst_name'].lower()].append('a constant')
        for struct in self.game['struct']:
            name = struct['str_name']
            uses[name.lower()].append('a struct name')
            for field_name, _, _ in struct['str_field']:
                uses[field_name.lower()].append(f'a field of struct "{name}"')
        for enum in self.game['enum']:
            name = enum['enum_name']
            uses[name.lower()].append('an enum name')
            for field_name, _ in enum['enum_field']:
                uses[field_name.lower()].append(f'a field of enum "{name}"')
        for func in (self.game['function'] + self.game['user_function']):
            name = func['fct_name']
            uses[name.lower()].append('a function name')
            self.check_func_args(func)

        for name in uses:
            if len(uses[name]) > 1:
                raise GameError(
                    f'Name "{name}" conflicts: {", ".join(uses[name])}'
                )

    def check_func_args(self, func):
        arg_names = set()
        for arg_name, arg_type, arg_comment in func['fct_arg']:
            if arg_name in self.types:
                raise GameError(
                    "Function '{}': Argument '{}' conflicts with a type name."
                    .format(func['fct_name'], arg_name))
            if arg_name in arg_names:
                raise GameError(
                    "Function '{}': Argument '{}' was defined twice."
                    .format(func['fct_name'], arg_name))
            arg_names.add(arg_name)

    def check_reserved_keywords(self) -> None:
        '''Check that no identifier is in conflict with a reserved keyword'''

        def throw_if_conflicts(name: str, what: str) -> bool:
            if name in RESERVED_LANGUAGE_KEYWORDS or name.capitalize(
            ) in RESERVED_LANGUAGE_KEYWORDS or iskeyword(name):
                raise GameError(
                    "{} '{}': name conflicts with a keyword of a target "
                    "language."
                    .format(what, name))
        throw_if_conflicts(self.game['name'], "Game name")
        for constant in self.game['constant']:
            throw_if_conflicts(constant['cst_name'], "Constant")
        for enum in self.game['enum']:
            throw_if_conflicts(enum['enum_name'], "Enum")
            for field_name, _ in enum['enum_field']:
                throw_if_conflicts(field_name,
                                   "Enum {}: field".format(enum['enum_name']))
        for struct in self.game['struct']:
            throw_if_conflicts(struct['str_name'], "Struct")
            for field_name, _, _ in struct['str_field']:
                throw_if_conflicts(
                    field_name, "Struct {}: field".format(struct['str_name']))
        for func in (self.game['function'] + self.game['user_function']):
            throw_if_conflicts(func['fct_name'], "Function")
            for arg_name, _, _ in func['fct_arg']:
                throw_if_conflicts(
                    arg_name, "Function {}: argument".format(func['fct_name']))

    def get_enum(self, enum_name):
        '''Get an enum by name, None if it does not exist'''
        return next(
            (
                enum
                for enum in self.game['enum']
                if enum['enum_name'] == enum_name
            ),
            None
        )

    def get_struct(self, struct_name):
        '''Get a struct by name, None if it does not exist'''
        return next(
            (
                struct
                for struct in self.game['struct']
                if struct['str_name'] == struct_name
            ),
            None
        )


# Adapted from camisole/schema.py
# ------------->8----------------


class GameSchemaError(GameError):
    def __init__(self, path, msg):
        self.path = path
        self.msg = msg
        super().__init__(str(self))

    def __str__(self):
        return f"{self.path}: {self.msg}"


class O:  # noqa: E742
    """Optional."""

    def __init__(self, wrapped):
        self.wrapped = wrapped

    def __repr__(self):
        return f"O[{self.wrapped}]"


class Union:
    """Any of."""

    def __init__(self, *wrapped):
        self.wrapped = wrapped

    def __repr__(self):
        return f"Union[{self.wrapped}]"


def validate_schema(obj, schema):
    def htn(cls):
        return {
            bytes: "binary data",
            int: "an integer",
            str: "a string",
            type(None): "nothing",
        }.get(cls, f"a {cls.__name__}")

    def explore(obj, schema, path):
        if isinstance(schema, O):
            if obj is None:
                return
            explore(obj, schema.wrapped, path)

        elif isinstance(schema, Union):
            for subtype in schema.wrapped:
                try:
                    explore(obj, subtype, path)
                    # one of the types is OK, early stop
                    return
                except GameSchemaError:
                    pass
            expected = ' or '.join(htn(s) for s in schema.wrapped)
            raise GameSchemaError(
                path, f"{path}: expected {expected}, got {htn(obj.__class__)}")

        elif isinstance(schema, list):
            subtype, = schema
            try:
                for i, item in enumerate(obj):
                    explore(item, subtype, f'{path}[{i}]')
            except TypeError:
                raise GameSchemaError(
                    path, f"expected a list, got {htn(obj.__class__)}")

        elif isinstance(schema, tuple):
            try:
                for i, item in enumerate(obj):
                    explore(item, schema[i], f'{path}[{i}]')
            except TypeError:
                raise GameSchemaError(
                    path, f"expected a list, got {htn(obj.__class__)}")

        elif isinstance(schema, dict):
            try:
                for key, subtype in schema.items():
                    explore(obj.get(key), subtype, f'{path}.{key}')
            except GameSchemaError:
                raise
            except Exception:
                raise GameSchemaError(
                    path, f"expected a mapping, got {htn(obj.__class__)}")

        elif isinstance(schema, typing.Pattern):
            if not isinstance(obj, str):
                raise GameSchemaError(
                    path, f"expected {htn(str)}, got {htn(obj.__class__)}")
            if not schema.match(obj):
                raise GameSchemaError(
                    path, f"{obj} does not match '{schema.pattern}'")

        elif not isinstance(obj, schema):
            raise GameSchemaError(
                path, f"expected {htn(schema)}, got {htn(obj.__class__)}")

    explore(obj, schema, '')

# -------------8<----------------


IDENTIFIER = re.compile(r'^[a-z][a-z0-9_]*$')
CONSTANT = re.compile(r'^[A-Z][A-Z0-9_]+$')
TYPE = re.compile(r'^[a-z][a-z0-9_]*( array)?$')

GAME_SCHEMA = {
    'name': IDENTIFIER,
    'rules_type': O(re.compile('(turnbased|synchronous)')),
    'constant': [
        {
            'cst_name': CONSTANT,
            'cst_val': Union(int, str, float),
            'cst_type': O(re.compile('(int|string|double)')),
            'cst_comment': str,
        }
    ],
    'enum': [
        {
            'enum_name': IDENTIFIER,
            'enum_summary': str,
            'enum_field': [(IDENTIFIER, str)],
        }
    ],
    'struct': [
        {
            'str_name': IDENTIFIER,
            'str_summary': str,
            'str_tuple': O(bool),
            'str_field': [(IDENTIFIER, TYPE, str)],
        }
    ],
    'function': [
        {
            'fct_name': IDENTIFIER,
            'fct_summary': str,
            'fct_ret_type': TYPE,
            'fct_arg': [(IDENTIFIER, TYPE, str)],
        }
    ],
    'user_function': [
        {
            'fct_name': IDENTIFIER,
            'fct_summary': str,
            'fct_ret_type': TYPE,
            'fct_arg': [(IDENTIFIER, TYPE, str)],
        }
    ],
}

# Obtained with sorted(set(iorgen.parser_c.KEYWORDS +
# iorgen.parser_cpp.KEYWORDS + iorgen.parser_csharp.KEYWORDS +
# iorgen.parser_haskell.KEYWORDS + iorgen.parser_java.KEYWORDS +
# iorgen.parser_php.KEYWORDS + iorgen.parser_rust.KEYWORDS))
# Note that python keywords are not present, use keyword.iskeyword
RESERVED_LANGUAGE_KEYWORDS = [
    'Self', 'abstract', 'alignas', 'alignof', 'and', 'and_eq', 'array', 'as',
    'asm', 'assert', 'atomic_cancel', 'atomic_commit', 'atomic_noexcept',
    'auto', 'base', 'become', 'bitand', 'bitor', 'bool', 'boolean', 'box',
    'break', 'byte', 'callable', 'case', 'catch', 'char', 'char16_t',
    'char32_t', 'checked', 'class', 'clone', 'co_await', 'co_return',
    'co_yield', 'compl', 'concept', 'const', 'const_cast', 'constexpr',
    'continue', 'crate', 'data', 'decimal', 'declare', 'decltype', 'default',
    'delegate', 'delete', 'deriving', 'die', 'do', 'double', 'dynamic_cast',
    'echo', 'else', 'elseif', 'empty', 'enddeclare', 'endfor', 'endforeach',
    'endif', 'endswitch', 'endwhile', 'enum', 'eval', 'event', 'exit',
    'explicit', 'export', 'extends', 'extern', 'extern(', 'false', 'final',
    'finally', 'fixed', 'float', 'fn', 'for', 'forall', 'foreach', 'foreign',
    'friend', 'function', 'global', 'goto', 'hiding', 'if', 'impl',
    'implements', 'implicit', 'import', 'in', 'include', 'include_once',
    'infix', 'infixl', 'infixr', 'inline', 'instance', 'instanceof',
    'insteadof', 'int', 'interface', 'internal', 'is', 'isset', 'let', 'list',
    'lock', 'long', 'loop', 'macro', 'match', 'mdo', 'mod', 'module', 'move',
    'mut', 'mutable', 'namespace', 'native', 'new', 'newtype', 'noexcept',
    'not', 'not_eq', 'null', 'nullptr', 'object', 'of', 'offsetof', 'operator',
    'or', 'or_eq', 'out', 'override', 'package', 'params', 'print', 'priv',
    'private', 'proc', 'protected', 'pub', 'public', 'pure', 'qualified',
    'readonly', 'rec', 'ref', 'reflexpr', 'register', 'reinterpret_cast',
    'require', 'require_once', 'requires', 'return', 'sbyte', 'sealed', 'self',
    'short', 'signed', 'sizeof', 'stackalloc', 'static', 'static_assert',
    'static_cast', 'strictfp', 'string', 'struct', 'super', 'switch',
    'synchronized', 'template', 'then', 'this', 'thread_local', 'throw',
    'throws', 'trait', 'transient', 'true', 'try', 'type', 'typedef', 'typeid',
    'typename', 'typeof', 'uint', 'ulong', 'unchecked', 'union', 'unsafe',
    'unset', 'unsigned', 'unsized', 'use', 'ushort', 'using', 'using static',
    'var', 'virtual', 'void', 'volatile', 'wchar_t', 'where', 'while', 'xor',
    'xor_eq', 'yield'
]
