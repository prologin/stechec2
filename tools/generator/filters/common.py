import jinja2

from . import register_filter, register_function


@register_filter
def camel_case(value: str) -> str:
    """Convert a snake case identifier to a upper camel case one"""
    return "".join(i.capitalize() for i in value.split("_"))


@register_function
def has_return(func) -> bool:
    return func['fct_ret_type'] != 'void'


@register_filter
def generic_args(value, type_mapper=lambda x: x) -> str:
    return ", ".join("{} {}".format(type_mapper(type_), name)
                     for [name, type_, _] in value)


@register_filter
def generic_prototype(func, arg_mapper=generic_args) -> str:
    return '{} {}({})'.format(
        func['fct_ret_type'], func['fct_name'],
        arg_mapper(func['fct_arg']))


@register_filter
@jinja2.environmentfilter
def generic_comment(env, value: str, start: str, indent: int = 0) -> str:
    newline = "\n" + indent * " " + start
    return start + env.call_filter("wordwrap", value,
                                   [79 - indent - len(start), False, newline])
