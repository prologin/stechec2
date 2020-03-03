from functools import partial
from jinja2 import contextfilter

from . import register_filter
from .common import generic_args, generic_prototype, is_struct, is_array, get_array_inner
from .cxx import cxx_comment, cxx_type


@register_filter
def c_type(value: str) -> str:
    if value == "string":
        return "char*"
    if value.endswith(" array"):
        return "{}_array".format(c_type(value[:-len(' array')]))
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

c_comment = register_filter(cxx_comment, name='c_comment')


# To avoid name clashing, we prefix the internal C++ structs
@register_filter
@contextfilter
def c_internal_cxx_type(ctx, value: str) -> str:
    if is_array(value):
        base_type = get_array_inner(value)
        return 'std::vector<{}>'.format(c_internal_cxx_type(ctx, base_type))
    elif is_struct(ctx, value):
        return '__internal__cxx__' + value
    return cxx_type(value)


@register_filter
@contextfilter
def c_to_cxx(ctx, value: str) -> str:
    if is_array(value):
        base_type = get_array_inner(value)
        return 'c_to_cxx_array<{}, {}, {}>'.format(
            c_type(base_type),
            c_type(value),
            c_internal_cxx_type(ctx, base_type)
        )
    else:
        ctype = c_type(value)
        cpptype = c_internal_cxx_type(ctx, value)
        return 'c_to_cxx<{}, {}>'.format(ctype, cpptype)


@register_filter
@contextfilter
def cxx_to_c(ctx, value: str) -> str:
    if is_array(value):
        base_type = get_array_inner(value)
        return 'cxx_to_c_array<{}, {}, {}>'.format(
            c_type(base_type),
            c_type(value),
            c_internal_cxx_type(ctx, base_type),
        )
    else:
        ctype = c_type(value)
        cpptype = c_internal_cxx_type(ctx, value)
        return 'cxx_to_c<{}, {}>'.format(ctype, cpptype)


@register_filter
@contextfilter
def c_internal_cxx_prototype(ctx, *args, **kwargs) -> str:
    return generic_prototype(type_mapper=partial(c_internal_cxx_type, ctx),
                             *args, **kwargs)
