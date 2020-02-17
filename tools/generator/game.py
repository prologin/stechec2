import yaml


class GameError(ValueError):
    pass


class Game:
    def __init__(self, game_file):
        self.game = yaml.safe_load(game_file)
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

        self.load_base_types()
        self.load_used_types()
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

    def check(self):
        '''Perform various integrity checks on the game objects'''
        for s in self.game['struct']:
            self.check_struct(s)
        for f in (self.game['function'] + self.game['user_function']):
            self.check_func(f)

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
        arg_names = set()
        for arg_name, arg_type, arg_comment in func['fct_arg']:
            if arg_name in self.types:
                raise GameError(
                    "Function '{}': Argument '{}' conflicts with a type name "
                    "(C code will not compile)."
                    .format(func['fct_name'], arg_name))
            if arg_name in arg_names:
                raise GameError(
                    "Function '{}': Argument '{}' was defined twice."
                    .format(func['fct_name'], arg_name))
            arg_names.add(arg_name)
