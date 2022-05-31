from functools import partial
try:
    from jinja2 import pass_context
except ImportError:  # jinja < 3
    from jinja2 import contextfilter as pass_context

from . import register_filter
from .common import generic_comment, get_array_inner, is_array, is_tuple
from .cxx import cxx_type


@register_filter
@pass_context
def python_type(ctx, val: str, tuple_alias: bool = True) -> str:
    base_types = {
        'void': 'None',
        'int': 'int',
        'double': 'float',
        'string': 'str',
    }
    if val in base_types:
        return base_types[val]
    if is_array(val):
        return 'List[{}]'.format(
            python_type(ctx, get_array_inner(val), tuple_alias)
        )
    s = ctx['game'].get_struct(val)
    if s and is_tuple(s) and tuple_alias:
        return 'Tuple[{}]'.format(
            ', '.join(
                python_type(ctx, arg_type, tuple_alias)
                for _, arg_type, _ in s['str_field']
            )
        )
    return val


@register_filter
@pass_context
def python_prototype(ctx, func, typed: bool = False) -> str:
    args_and_types = [
        (arg_name, python_type(ctx, arg_type, False))
        for arg_name, arg_type, _ in func['fct_arg']
    ]
    if typed:
        return 'def {}({}) -> {}:'.format(
            func['fct_name'],
            ', '.join(f'{i}: {j}' for i, j in args_and_types),
            python_type(ctx, func['fct_ret_type'], False),
        )
    return 'def {}({}):'.format(
        func['fct_name'], ', '.join(i for i, _ in args_and_types)
    )


python_comment = register_filter(
    partial(generic_comment, start='# '),
    name='python_comment',
)


@register_filter
def python_docstring(value: str, indent: int = 0) -> str:
    ind = ' ' * indent
    return '"""\n' + generic_comment(value, ind) + '\n' + ind + '"""'


@register_filter
def cxx_to_python(value: str) -> str:
    if is_array(value):
        return 'cxx_to_python_array'
    else:
        return 'cxx_to_python<PyObject*, {}>'.format(cxx_type(value))


@register_filter
def python_to_cxx(value: str) -> str:
    if is_array(value):
        return 'python_to_cxx_array<{}>'.format(
            cxx_type(get_array_inner(value))
        )
    else:
        return 'python_to_cxx<PyObject*, {}>'.format(cxx_type(value))


@register_filter
def get_python_import(functions) -> str:
    """Get all needed imports to be able to declare some functions"""

    all_types = set()
    for func in functions:
        for _, arg_type, _ in func['fct_arg']:
            all_types.add(arg_type)
        all_types.add(func['fct_ret_type'])
    import_api = set()
    import_typing = set()
    for val in all_types:
        while is_array(val):
            import_typing.add('List')
            val = get_array_inner(val)
        if val not in ('void', 'int', 'double', 'string', 'bool'):
            import_api.add(val)
    ret = ''
    if import_typing:
        ret += f'from typing import {", ".join(sorted(import_typing))}'
    if import_typing and import_api:
        ret += "\n\n"
    if import_api:
        ret += f'from api import {", ".join(sorted(import_api))}'
    return ret
