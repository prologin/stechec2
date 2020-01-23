import functools
from .common import generic_args, generic_prototype


def c_type(value: str) -> str:
    if value == "string":
        return "char*"
    if value.endswith(" array"):
        return "{}_array".format(c_type(value[:-len('_array')]))
    return value


c_args = functools.partial(generic_args, type_mapper=c_type)
c_prototype = functools.partial(generic_prototype, arg_mapper=c_args)
