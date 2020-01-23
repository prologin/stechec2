import jinja2
from . import register_filter
from .common import generic_args, generic_prototype, generic_comment


@register_filter
def c_type(value: str) -> str:
    if value == "string":
        return "char*"
    if value.endswith(" array"):
        return "{}_array".format(c_type(value[:-len('_array')]))
    return value


@register_filter
def c_args(value):
    return generic_args(value, type_mapper=c_type)


@register_filter
def c_prototype(value):
    return generic_prototype(value, arg_mapper=c_args)


@register_filter
@jinja2.environmentfilter
def c_comment(env, value, indent: int = 0):
    return generic_comment(env, value=value, start='// ', indent=indent)
