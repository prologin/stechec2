from functools import partial
from jinja2 import contextfilter

from . import register_filter
from .common import generic_args, generic_prototype, is_struct_in, is_array
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
    if is_struct_in(value, ctx['game']):
        return '__internal__cxx__' + value
    return cxx_type(value)


@register_filter
@contextfilter
def c_lang2cxx(ctx, value: str) -> str:
    ctype = c_type(value)
    cpptype = c_internal_cxx_type(ctx, value)
    if is_array(value):
        return 'lang2cxx_array<{}, {}, {}>'.format(
            ctype[:-len('_array')],
            cpptype,
            cpptype[len('std::vector<'):len('>')]
        )
    else:
        return 'lang2cxx<{}, {}>'.format(ctype, cpptype)


@register_filter
@contextfilter
def c_cxx2lang(ctx, value: str) -> str:
    ctype = c_type(value)
    cpptype = c_internal_cxx_type(ctx, value)
    if is_array(value):
        return 'cxx2lang_array<{}, {}, {}>'.format(
            ctype[:-len('_array')],
            ctype,
            cpptype[len('std::vector<'):-len('>')]
        )
    else:
        return 'cxx2lang<{}, {}>'.format(ctype, cpptype)


@register_filter
@contextfilter
def c_internal_cxx_prototype(ctx, *args, **kwargs) -> str:
    return generic_prototype(type_mapper=partial(c_internal_cxx_type, ctx),
                             *args, **kwargs)
