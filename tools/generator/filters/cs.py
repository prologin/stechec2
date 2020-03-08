from functools import partial

from . import register_filter
from .common import camel_case, generic_comment, get_array_inner, is_array


def cs_type(type) -> str:
    if is_array(type):
        inner = get_array_inner(type)
        return f'{inner}[]'


@register_filter
def cs_prototype(func) -> str:
    # output type
    ret_type = func['fct_ret_type']
    if is_array(ret_type):
        ret_type = 'void'

    # input args
    args = ['{} {}'.format(cs_type(arg_type), arg_name)
            for arg_name, arg_type, _ in func['fct_arg']]

    if func['fct_ret_type']:
        args.append('{} ret_arr'.format(cs_type(func['fct_ret_type'])))

    args = ', '.join(args)

    return 'public {} {}({})'.format(
                            ret_type,
                            camel_case(func['fct_name']),
                            args
        )


cs_comment = register_filter(
    partial(generic_comment, start='// '),
    name='cs_comment',
)
