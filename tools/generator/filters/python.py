from functools import partial
from jinja2 import contextfilter

from . import register_filter
from .common import generic_comment, get_array_inner, is_array, is_tuple
from .cxx import cxx_type


@register_filter
@contextfilter
def python_type(ctx, val: str) -> str:
    base_types = {
        'void': 'None',
        'int': 'int',
        'double': 'float',
        'string': 'str',
    }
    if val in base_types:
        return base_types[val]
    if is_array(val):
        return 'List[{}]'.format(python_type(ctx, get_array_inner(val)))
    s = ctx['game'].get_struct(val)
    if s and is_tuple(s):
        return 'Tuple[{}]'.format(', '.join(
            python_type(ctx, arg_type) for _, arg_type, _ in s['str_field']
        ))
    return val


@register_filter
def python_prototype(func) -> str:
    return 'def {}({}):'.format(
        func['fct_name'],
        ', '.join(arg_name for arg_name, _, _ in func['fct_arg'])
    )


python_comment = register_filter(
    partial(generic_comment, start='# '),
    name='python_comment',
)


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
