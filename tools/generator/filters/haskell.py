from functools import partial

from . import register_filter
from .common import generic_comment, get_array_inner, is_array


@register_filter
def haskell_type(type_id: str) -> str:
    if is_array(type_id):
        return '[' + haskell_type(get_array_inner(type_id)) + ']'
    elif type_id == 'void':
        return '()'
    return type_id.capitalize()


haskell_comment = register_filter(
    partial(generic_comment, start='-- '),
    name='haskell_comment',
)
