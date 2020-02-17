import textwrap
from functools import partial
from jinja2 import contextfunction

from . import register_filter, register_test, register_function


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
    return is_struct_in(type, ctx['game'])


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


def game_types(game):
    return {
        *[s['enum_name'] for s in game['enum']],
        *[s['str_name'] for s in game['struct']],
        *[t for s in game['struct'] for _, t, _ in s['str_field']],
        *[s['fct_ret_type']
          for s in (game['function'] + game['user_function'])],
        *[t for f in (game['function'] + game['user_function'])
          for _, t, _ in f['fct_arg']],
    }


@register_function
@contextfunction
def array_types(ctx):
    return {
        t[:-len(' array')]
        for t in game_types(ctx['game']) if t.endswith(' array')
    }
