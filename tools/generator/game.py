import re
import typing
import yaml


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
        self.load_base_types()
        self.load_used_types()
        self.load_funcs()
        self.check()

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
        # TODO: add display_* functions

    def check(self):
        '''Perform various integrity checks on the game objects'''
        for s in self.game['struct']:
            self.check_struct(s)
        for f in (self.game['function'] + self.game['user_function']):
            self.check_func(f)
        self.check_field_unicity()

    def check_struct(self, struct):
        field_names = set()
        for field_name, field_type, field_comment in struct['str_field']:
            if field_name in self.types:
                raise GameError(
                    "Struct {}: Field '{}' conflicts with a type name "
                    "(C code will not compile)."
                    .format(struct['str_name'], field_name))
            if field_name in field_names:
                raise GameError(
                    "Struct {}: Field '{}' was defined twice."
                    .format(struct['str_name'], field_name))
            field_names.add(field_name)

    def check_func(self, func):
        if func['fct_name'] in self.types:
            raise GameError(
                "Function '{}': name conflicts with a type name."
                .format(func['fct_name']))
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

    def check_field_unicity(self):
        '''
        Check that the enum and struct field names are unique.

        Some languages like OCaml infer types from the name of the enum and
        struct fields, and thus require them to be unique *across* different
        types. This rejects types that share a common field name.
        '''
        merged_types = [
            *[('enum', e['enum_name'], [fn for fn, _ in e['enum_field']])
              for e in self.game['enum']],
            *[('struct', e['str_name'], [fn for fn, _, _ in e['str_field']])
              for e in self.game['struct']]
        ]

        used_field_names = {}  # field_name -> origin type
        for origin_type, origin, field_names in merged_types:
            for name in field_names:
                if name in used_field_names:
                    def_type, def_name = used_field_names[name]
                    raise GameError(
                        "{} '{}': Field name '{}' already used in {} '{}'. "
                        "This will confuse some type inferrers."
                        .format(origin_type, origin, name, def_type, def_name))
                used_field_names[name] = (origin_type, origin)

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
            'cst_val': Union(int, float),
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
