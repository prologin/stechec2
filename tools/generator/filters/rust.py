import string
from functools import partial
from jinja2 import contextfilter

from . import register_filter
from .common import (
    camel_case, generic_args, generic_comment, get_array_inner, is_array,
    is_enum, is_returning, is_struct, is_tuple,
)
from .cxx import cxx_comment


register_filter(cxx_comment, name='rust_comment')


@register_filter
@contextfilter
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
@contextfilter
def rust_prototype(ctx, func, ffi=False) -> str:
    # Input params
    get_type = rust_ffi_type if ffi else rust_api_input_type
    args = [
        '{}: {}'.format(arg_name, get_type(ctx, arg_type))
        for arg_name, arg_type, _ in func['fct_arg']
    ]

    # Output
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
@contextfilter
def rust_ffi_type(ctx, value: str) -> str:
    """Type sent to the FFI"""
    generics = {
        'bool': 'c_bool',
        'double': 'c_double',
        'int': 'c_int',
        'string': 'RawString',
        'void': '()',
    }

    if is_array(value):
        return 'Array<{}>'.format(rust_ffi_type(ctx, get_array_inner(value)))
    elif value in generics:
        return generics[value]
    else:
        return camel_case(value)


@register_filter
@contextfilter
def rust_api_output_type(ctx, value: str, api_mod_path='') -> str:
    generics = {
        'bool': 'bool',
        'double': 'f64',
        'int': 'i32',
        'void': '()',
    }

    if value == 'string':
        return 'String'
    elif is_array(value):
        return 'Vec<{}>'.format(rust_api_output_type(ctx, get_array_inner(value)))
    elif value in generics:
        return generics[value]
    elif is_struct(ctx, value) and is_tuple(ctx['game'].get_struct(value)):
        return '({})'.format(
            ', '.join(
                rust_api_output_type(ctx, field)
                for _, field, _ in ctx['game'].get_struct(value)['str_field']
            ))
    else:
        return api_mod_path + camel_case(value)


@register_filter
@contextfilter
def rust_api_input_type(ctx, value: str, api_mod_path='', skip_ref=False) -> str:
    if value == 'string':
        return '&str'
    elif is_array(value):
        return '&[' + rust_api_output_type(ctx, get_array_inner(value)) + ']'
    elif is_struct(ctx, value) and is_tuple(ctx['game'].get_struct(value)):
        return '({})'.format(
            ', '.join(
                rust_api_input_type(ctx, field)
                for _, field, _ in ctx['game'].get_struct(value)['str_field']
            ))
    elif not rust_is_copy(ctx, value):
        if skip_ref:
            return rust_api_output_type(ctx, value, api_mod_path)
        else:
            return '&' + rust_api_output_type(ctx, value, api_mod_path)
    else:
        return rust_api_output_type(ctx, value, api_mod_path)


@register_filter
@contextfilter
def rust_is_copy(ctx, value: str) -> bool:
    """Check if a type implements the Copy trait"""
    if is_struct(ctx, value) and is_tuple(ctx['game'].get_struct(value)):
        return all(
            rust_is_copy(ctx, field_type)
            for _, field_type, _ in ctx['game'].get_struct(value)['str_field']
        )

    return (
        is_enum(ctx, value) or value in ['bool', 'double', 'int', 'void']
    )


@register_filter
@contextfilter
def rust_auto_traits(ctx, value: str) -> set:
    """
    Return the list of auto traits that can be implemented for given input
    type.
    """
    if is_struct(ctx, value):
        inherited = set.intersection(*(
            rust_auto_traits(ctx, field_type)
            for _, field_type, _ in ctx['game'].get_struct(value)['str_field']
        ))

        if not is_tuple(ctx['game'].get_struct(value)) and 'Copy' in inherited:
            inherited.remove('Copy')

        return inherited

    if is_array(value):
        inherited = rust_auto_traits(ctx, get_array_inner(value))

        if 'Copy' in inherited:
            inherited.remove('Copy')

        return inherited

    if value == 'double':
        return {'Copy', 'Clone', 'Debug', 'PartialEq', 'PartialOrd'}

    return {'Copy', 'Clone', 'Debug', 'Eq', 'Hash', 'Ord', 'PartialEq',
            'PartialOrd'}
