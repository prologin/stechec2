from functools import partial

from . import register_filter
from .common import generic_comment, get_array_inner, is_array


@register_filter
def haskell_type(type_id: str) -> str:
    if is_array(type_id):
        return f'[{haskell_type(get_array_inner(type_id))}]'
    elif type_id == 'void':
        return '()'
    return type_id.capitalize()


@register_filter
def haskell_c_type(type_id: str) -> str:
    if is_array(type_id):
        return f'{haskell_c_type(get_array_inner(type_id))}_array'
    return type_id.capitalize()


haskell_comment = register_filter(
    partial(generic_comment, start='-- '),
    name='haskell_comment',
)


@register_filter
def haskell_get_array_types(game: dict):
    types = list()

    def add_array_type(types, type_id):
        if is_array(type_id):
            types.append((haskell_c_type(type_id), type_id))
            add_array_type(types, get_array_inner(type_id))
        return

    for s in game['struct']:
        for _, type_id, _ in s['str_field']:
            add_array_type(types, type_id)

    for f in game['function'] + game['user_function']:
        add_array_type(types, f['fct_ret_type'])
        for arg in f['fct_arg']:
            add_array_type(types, arg[1])

    types = list(dict.fromkeys(types))
    return types
