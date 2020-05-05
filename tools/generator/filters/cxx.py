from functools import partial

from . import register_filter
from .common import generic_args, generic_prototype, generic_comment


@register_filter
def cxx_type(value: str) -> str:
    if value == "string":
        return "std::string"
    if value.endswith(" array"):
        return "std::vector<{}>".format(cxx_type(value[:-len(" array")]))
    return value


cxx_args = register_filter(
    partial(generic_args, type_mapper=cxx_type),
    name='cxx_args',
)
cxx_prototype = register_filter(
    partial(generic_prototype, type_mapper=cxx_type),
    name='cxx_prototype',
)


@register_filter
def cxx_func_ptr(func) -> str:
    return '{} (*)({})'.format(
        cxx_type(func['fct_ret_type']),
        generic_args(func['fct_arg'], cxx_type))


@register_filter
def cxx_comment(*args, doc: bool = False, **kwargs):
    start = "/// " if doc else "// "
    return generic_comment(*args, **kwargs, start=start)
