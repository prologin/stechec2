from functools import partial

from . import register_filter
from .common import generic_comment, get_array_inner, is_array


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
        return 'cxx_to_python<PyObject*, {}>'.format(value)


@register_filter
def python_to_cxx(value: str) -> str:
    if is_array(value):
        return 'python_to_cxx_array<{}>'.format(get_array_inner(value))
    else:
        return 'python_to_cxx<PyObject*, {}>'.format(value)
