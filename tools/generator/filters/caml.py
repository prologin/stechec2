import textwrap

from . import register_filter, register_test
from .common import get_array_inner, is_array
from .cxx import cxx_type


@register_filter
def caml_type(value: str) -> str:
    if value == 'void':
        return 'unit'
    if value == 'double':
        return 'float'
    if is_array(value):
        return '{} array'.format(caml_type(get_array_inner(value)))
    return value


# caml functions need to take at least one parameter, be it a unit value if
# there is nothing useful to give

@register_filter
def caml_cxx_args(arg_list):
    if not arg_list:
        return [('unit', 'void', 'empty caml parameter')]
    return arg_list


@register_filter
def caml_prototype(func) -> str:
    arg_list = ' '.join("({}:{})".format(arg_name, caml_type(arg_type))
                        for arg_name, arg_type, _ in func['fct_arg'])
    if arg_list == '':  # make unit the only parameter
        arg_list = '()'

    return 'let {} {} : {} ='.format(func['fct_name'],
                                     arg_list,
                                     caml_type(func['fct_ret_type']))


@register_filter
def caml_signature(func, external: bool = False) -> str:
    arg_list = ' -> '.join(caml_type(arg_type)
                           for _, arg_type, _
                           in func['fct_arg'])
    if arg_list == '':  # make unit the only parameter
        arg_list = 'unit'

    return '{} {} : {} -> {}'.format('external' if external else 'val',
                                     func['fct_name'],
                                     arg_list,
                                     caml_type(func['fct_ret_type']))


@register_filter
def caml_comment(value: str, indent: int = 0, doc: bool = False) -> str:
    # Make sure start and multiline_start are the same length
    start = '(** ' if doc else '(* '
    multiline_start = ' ** ' if doc else '** '
    newline = "\n" + indent * " " + multiline_start

    return start + newline.join(textwrap.wrap(
        value + ' *)',
        79 - indent - len(multiline_start),
        expand_tabs=False,
        break_long_words=False,
        replace_whitespace=False
    ))


@register_filter
def cxx_to_caml(value: str) -> str:
    if is_array(value):
        return 'cxx_to_caml_array'
    else:
        return 'cxx_to_caml<value, {}>'.format(cxx_type(value))


@register_filter
def caml_to_cxx(value: str) -> str:
    if is_array(value):
        return 'caml_to_cxx_array<{}>'.format(cxx_type(get_array_inner(value)))
    else:
        return 'caml_to_cxx<value, {}>'.format(cxx_type(value))


@register_filter
def caml_param_macro(value) -> str:
    """
    Generate the series of CAMLparamX macros corresponding to the given
    parameter list by grouping the parameters in chunks of 5.
    """
    params = list(value)
    lines = []
    for ndx in range(0, len(params), 5):
        chunk = params[ndx:min(ndx + 5, len(params))]
        lines.append('{}{}({});'.format(
            'CAMLparam' if ndx == 0 else 'CAMLxparam',
            len(chunk),
            ', '.join(chunk)
        ))
    return '\n'.join(lines)


@register_test
def caml_test_float_struct(struct) -> bool:
    """
    Test if the struct contain only float fields.
    """
    return all(type_value == "double"
               for _, type_value, _ in struct["str_field"])
