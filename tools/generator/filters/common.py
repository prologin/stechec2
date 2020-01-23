def camel_case(value: str) -> str:
    """Convert a snake case identifier to a upper camel case one"""
    return "".join(i.capitalize() for i in value.split("_"))


def generic_args(value, type_mapper=lambda x: x) -> str:
    return ", ".join("{} {}".format(type_mapper(type_), name)
                     for [name, type_, _] in value)


def generic_prototype(func, arg_mapper=generic_args) -> str:
    return '{} {}({})'.format(
        func['fct_ret_type'], func['fct_name'],
        arg_mapper(func['fct_arg']))
