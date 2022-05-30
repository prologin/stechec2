from functools import partial
try:
    from jinja2 import pass_context
except ImportError:  # jinja < 3
    from jinja2 import contextfilter as pass_context

from . import register_filter
from .c import c_type, c_args, c_internal_cxx_type
from .common import (generic_comment, generic_prototype, get_array_inner,
                     is_array)


@register_filter
def haskell_type(type_id: str) -> str:
    if is_array(type_id):
        return f'[{haskell_type(get_array_inner(type_id))}]'
    elif type_id == 'void':
        return '()'
    return type_id.capitalize()


@register_filter
def haskell_c_type(type_id: str) -> str:
    if is_array(type_id):
        return f'{haskell_c_type(get_array_inner(type_id))}_array'
    return type_id.capitalize()


@register_filter
@pass_context
def cptr_type(ctx, type_id: str) -> str:
    res = c_type(type_id)
    if is_array(type_id) or ctx['game'].get_struct(type_id):
        res += '*'
    return res


haskell_comment = register_filter(
    partial(generic_comment, start='-- '),
    name='haskell_comment',
)


@register_filter
def haskell_get_array_types(game: dict):
    types = list()

    def add_array_type(types, type_id):
        if is_array(type_id):
            types.append((haskell_c_type(type_id), type_id))
            add_array_type(types, get_array_inner(type_id))
        return

    for s in game['struct']:
        for _, type_id, _ in s['str_field']:
            add_array_type(types, type_id)

    for f in game['function'] + game['user_function']:
        add_array_type(types, f['fct_ret_type'])
        for arg in f['fct_arg']:
            add_array_type(types, arg[1])

    types = list(dict.fromkeys(types))
    return types


@register_filter
@pass_context
def cptr_to_cxx(ctx, value: str, use_ptr: bool = False) -> str:
    if is_array(value):
        base_type = get_array_inner(value)
        return 'cptr_to_cxx_array{}<{}, {}, {}>'.format(
            '_ptr' if use_ptr else '',
            c_type(base_type),
            c_type(value),
            c_internal_cxx_type(ctx, base_type)
        )
    else:
        ctype = cptr_type(ctx, value) if use_ptr else c_type(value)
        cpptype = c_internal_cxx_type(ctx, value)
        return 'cptr_to_cxx<{}, {}>'.format(ctype, cpptype)


@register_filter
@pass_context
def cxx_to_cptr(ctx, value: str, use_ptr: bool = False) -> str:
    if is_array(value):
        base_type = get_array_inner(value)
        return 'cxx_to_cptr_array{}<{}, {}, {}>'.format(
            '_ptr' if use_ptr else '',
            c_type(base_type),
            c_type(value),
            c_internal_cxx_type(ctx, base_type),
        )
    else:
        ctype = cptr_type(ctx, value) if use_ptr else c_type(value)
        cpptype = c_internal_cxx_type(ctx, value)
        return 'cxx_to_cptr<{}, {}>'.format(ctype, cpptype)


@register_filter
@pass_context
def cptr_internal_cxx_prototype(ctx, *args, **kwargs) -> str:
    return generic_prototype(type_mapper=partial(c_internal_cxx_type, ctx),
                             *args, **kwargs)


@register_filter
@pass_context
def haskell_c_prototype(ctx, *args, **kwargs) -> str:
    type_mapper = partial(cptr_type, ctx)
    arg_mapper = partial(c_args, type_mapper=type_mapper)
    return generic_prototype(type_mapper=type_mapper,
                             arg_mapper=arg_mapper,
                             prefix='hs_', *args, **kwargs)
