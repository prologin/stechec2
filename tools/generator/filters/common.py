import textwrap
from functools import partial
from jinja2 import contextfunction

from . import register_filter, register_test, register_function

SIMPLE_TYPES = ('int', 'double', 'string')


@register_filter
def camel_case(value: str) -> str:
    """Convert a snake case identifier to a upper camel case one"""
    return "".join(i.capitalize() for i in value.split("_"))


@register_test
def is_returning(func) -> bool:
    return func['fct_ret_type'] != 'void'


@register_test
def is_array(type) -> bool:
    return type.endswith(' array')


def is_struct_in(type, game) -> bool:
    return any(type == s['str_name'] for s in game['struct'])


@register_function
@contextfunction
def is_struct(ctx, type) -> bool:
    return any(type == s['str_name'] for s in ctx['game']['struct'])


@register_filter
def generic_args(value, type_mapper=lambda x: x) -> str:
    return ", ".join("{} {}".format(type_mapper(type_), name)
                     for [name, type_, _] in value)


@register_filter
def generic_prototype(func, prefix='', type_mapper=lambda x: x,
                      arg_mapper=None) -> str:
    if arg_mapper is None:
        arg_mapper = partial(generic_args, type_mapper=type_mapper)
    return '{} {}{}({})'.format(
        type_mapper(func['fct_ret_type']), prefix, func['fct_name'],
        arg_mapper(func['fct_arg']))


@register_filter
def generic_comment(value: str, start: str, indent: int = 0) -> str:
    newline = "\n" + indent * " " + start
    return start + newline.join(textwrap.wrap(
        value,
        79 - indent - len(start),
        expand_tabs=False,
        break_long_words=False,
        replace_whitespace=False
    ))


@register_function
def array_types(game):
    return SIMPLE_TYPES + tuple([s['str_name'] for s in game['struct']])
