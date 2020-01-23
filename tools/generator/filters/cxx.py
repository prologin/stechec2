import jinja2

from . import register_filter
from .common import generic_args, generic_prototype, generic_comment


@register_filter
def cxx_type(value: str) -> str:
    if value == "string":
        return "std::string"
    if value.endswith(" array"):
        return "std::vector<{}>".format(cxx_type(value[:-6]))
    return value


@register_filter
def cxx_args(value):
    return generic_args(value, type_mapper=cxx_type)


@register_filter
def cxx_prototype(value):
    return generic_prototype(value, arg_mapper=cxx_args)


@register_filter
@jinja2.environmentfilter
def cxx_comment(env, value, doc: bool = False, indent: int = 0):
    start = "/// " if doc else "// "
    return generic_comment(env, value=value, start=start, indent=indent)
