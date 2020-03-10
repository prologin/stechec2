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
def c_haskell_type(type_id: str) -> str:
    def recurse_inner(type_id: str) -> str:
        if is_array(type_id):
            return f'{recurse_inner(get_array_inner(type_id))}_array'
        return type_id.capitalize()
    return f'C{recurse_inner(type_id)}'


haskell_comment = register_filter(
    partial(generic_comment, start='-- '),
    name='haskell_comment',
)
