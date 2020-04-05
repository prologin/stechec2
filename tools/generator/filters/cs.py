from functools import partial
from jinja2 import contextfilter

from . import register_filter
from .common import (generic_comment, get_array_inner, generic_prototype,
                     is_array, camel_case)
from .cxx import cxx_type


@register_filter
@contextfilter
def cs_type(ctx, value) -> str:
    if is_array(value):
        return '{}[]'.format(cs_type(ctx, get_array_inner(value)))
    elif ctx['game'].get_struct(value) or ctx['game'].get_enum(value):
        return camel_case(value)
    return value


cs_comment = register_filter(
    partial(generic_comment, start='// '),
    name='cs_comment',
)


@register_filter
@contextfilter
def cs_prototype(ctx, value, *args, **kwargs) -> str:
    value = value.copy()
    value['fct_name'] = camel_case(value['fct_name'])
    return generic_prototype(value, *args, type_mapper=partial(cs_type, ctx),
                             **kwargs)


@register_filter
@contextfilter
def cs_mono_type(ctx, value):
    if is_array(value):
        return 'MonoArray*'
    elif ctx['game'].get_struct(value):
        return 'MonoObject*'
    elif ctx['game'].get_enum(value):
        return 'gint32'
    else:
        return {
            'void': 'void',
            'int': 'gint32',
            'double': 'double',
            'bool': 'gint32',
            'string': 'MonoString',
        }[value]


@register_filter
@contextfilter
def cs_mono_prototype(ctx, *args, **kwargs) -> str:
    return generic_prototype(type_mapper=partial(cs_mono_type, ctx),
                             *args, prefix='cs_', **kwargs)


@register_filter
@contextfilter
def cs_to_cxx(ctx, value: str) -> str:
    if is_array(value):
        base_type = get_array_inner(value)
        cstype = cs_mono_type(ctx, base_type)
        cxxtype = cxx_type(base_type)
        return 'lang2cxx_array<{}, {}>'.format(cstype, cxxtype)
    else:
        cstype = cs_mono_type(ctx, value)
        cxxtype = cxx_type(value)
        return 'lang2cxx<{}, {}>'.format(cstype, cxxtype)


@register_filter
@contextfilter
def cxx_to_cs(ctx, value: str) -> str:
    if is_array(value):
        base_type = get_array_inner(value)
        cstype = cs_mono_type(ctx, base_type)
        cxxtype = cxx_type(base_type)
        return 'cxx2lang_array<{}, {}>'.format(cstype, cxxtype)
    else:
        cstype = cs_mono_type(ctx, value)
        cxxtype = cxx_type(value)
        return 'cxx2lang<{}, {}>'.format(cstype, cxxtype)
