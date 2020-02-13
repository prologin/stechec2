from functools import partial

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
def c_args(value, type_mapper=lambda x: x) -> str:
    if not value:
        return 'void'
    else:
        return generic_args(value, type_mapper=c_type)


c_prototype = register_filter(
    partial(generic_prototype, type_mapper=c_type, arg_mapper=c_args),
    name='c_prototype',
)
c_comment = register_filter(
    partial(generic_comment, start='//'),
    name='c_comment',
)
