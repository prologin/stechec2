from functools import partial

from . import register_filter
from .common import generic_comment, is_array


@register_filter
def php_prototype(func) -> str:
    return 'function {}({})'.format(
        func['fct_name'],
        ', '.join(arg_name for arg_name, _, _ in func['fct_arg'])
    )


php_comment = register_filter(
    partial(generic_comment, start='// '),
    name='php_comment',
)


@register_filter
def cxx_to_php(value: str) -> str:
    if is_array(value):
        return 'cxx_to_php_array'
    else:
        return 'cxx_to_php<zval, {}>'.format(value)


@register_filter
def php_to_cxx(value: str) -> str:
    if is_array(value):
        return 'php_to_cxx_array<{}>'.format(value[:-len(' array')])
    else:
        return 'php_to_cxx<zval*, {}>'.format(value)
