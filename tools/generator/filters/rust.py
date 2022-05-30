try:
    from jinja2 import pass_context
except ImportError:  # jinja < 3
    from jinja2 import contextfilter as pass_context

from . import register_filter
from .common import (
    camel_case, get_array_inner, is_array, is_returning, is_tuple,
)
from .cxx import cxx_comment


register_filter(cxx_comment, name='rust_comment')


@register_filter
@pass_context
def rust_ffi_call(ctx, func) -> str:
    """
    Call FFI function, assuming that non-copy types are wrapped in a
    UnsafeCell.
    """
    def arg_wrapper(arg, type):
        if rust_is_copy(ctx, type):
            return arg
        else:
            return arg + '.get().read()'

    args = (
        arg_wrapper(arg_name, arg_type)
        for arg_name, arg_type, _ in func['fct_arg']
    )

    return '{}({})'.format(func['fct_name'], ', '.join(args))


@register_filter
@pass_context
def rust_prototype(ctx, func, ffi=False) -> str:
    # Input params
    get_type = rust_ffi_type if ffi else rust_api_input_type
    args = [
        '{}: {}'.format(arg_name, get_type(ctx, arg_type))
        for arg_name, arg_type, _ in func['fct_arg']
    ]

    # Output
    get_type = rust_ffi_type if ffi else rust_api_output_type

    if is_returning(func):
        ret_annotation = ' -> ' + get_type(ctx, func['fct_ret_type'])
    else:
        ret_annotation = ''

    return 'fn {}({}){}'.format(
        func['fct_name'],
        ', '.join(args),
        ret_annotation,
    )


@register_filter
@pass_context
def rust_ffi_type(ctx, value: str) -> str:
    """Type sent to the FFI"""
    basic_types = {
        'bool': 'c_bool',
        'double': 'c_double',
        'int': 'c_int',
        'string': 'RawString',
        'void': '()',
    }

    if is_array(value):
        return 'Array<{}>'.format(rust_ffi_type(ctx, get_array_inner(value)))
    elif value in basic_types:
        return basic_types[value]
    else:
        return camel_case(value)


@register_filter
@pass_context
def rust_api_output_type(ctx, value: str, api_mod_path='') -> str:
    basic_types = {
        'bool': 'bool',
        'double': 'f64',
        'int': 'i32',
        'string': 'String',
        'void': '()',
    }

    if is_array(value):
        return 'Vec<{}>'.format(
            rust_api_output_type(ctx, get_array_inner(value), api_mod_path)
        )
    elif value in basic_types:
        return basic_types[value]
    else:
        return api_mod_path + camel_case(value)


@register_filter
@pass_context
def rust_tuple_type(ctx, value: str, api_mod_path='') -> str:
    tup = ctx['game'].get_struct(value)
    assert is_tuple(tup), "{} is not a tuple struct".format(value)
    return '({})'.format(
        ', '.join(
            rust_api_output_type(ctx, field, api_mod_path)
            for _, field, _ in ctx['game'].get_struct(value)['str_field']
        ))


@register_filter
@pass_context
def rust_api_input_type(
    ctx, value: str, api_mod_path='', skip_ref=False
) -> str:
    if value == 'string':
        return '&str'
    elif is_array(value):
        if get_array_inner(value) == 'string':
            return '&[impl AsRef<str>]'
        else:
            return '&[impl Borrow<{}>]'.format(
                rust_api_input_type(
                    ctx, get_array_inner(value), api_mod_path, True
                )
            )
    elif rust_is_copy(ctx, value):
        return rust_api_output_type(ctx, value, api_mod_path)
    else:
        res = rust_api_output_type(ctx, value, api_mod_path)

        if not skip_ref:
            res = '&' + res

        return res


@register_filter
def rust_api_const_type(value: str) -> str:
    const_types = {
        'double': 'f64',
        'int': 'i32',
        'string': "&'static str",
    }
    return const_types[value]


@register_filter
@pass_context
def rust_is_copy(ctx, value: str) -> bool:
    """Check if a type implements the Copy trait"""
    as_struct = ctx['game'].get_struct(value)

    if as_struct and is_tuple(as_struct):
        return all(
            rust_is_copy(ctx, field_type)
            for _, field_type, _ in ctx['game'].get_struct(value)['str_field']
        )

    return (
        ctx['game'].get_enum(value) is not None
        or value in ['bool', 'double', 'int', 'void']
    )


@register_filter
@pass_context
def rust_auto_traits(ctx, value: str) -> set:
    """
    Return the list of auto traits that can be implemented for given input
    type.
    """
    as_struct = ctx['game'].get_struct(value)

    if as_struct:
        return set.intersection(*(
            rust_auto_traits(ctx, field_type)
            for _, field_type, _ in as_struct['str_field']
        ))

    if is_array(value):
        inherited = rust_auto_traits(ctx, get_array_inner(value))

        if 'Copy' in inherited:
            inherited.remove('Copy')

        return inherited

    if value == 'double':
        return {'Copy', 'Clone', 'Debug', 'PartialEq', 'PartialOrd'}

    if value == 'string':
        return {'Clone', 'Debug', 'Eq', 'Hash', 'Ord', 'PartialEq',
                'PartialOrd'}

    return {'Copy', 'Clone', 'Debug', 'Eq', 'Hash', 'Ord', 'PartialEq',
            'PartialOrd'}
