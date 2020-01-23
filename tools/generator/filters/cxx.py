import jinja2
import functools
from .common import generic_args, generic_prototype


def cxx_type(value: str) -> str:
    if value == "string":
        return "std::string"
    if value.endswith(" array"):
        return "std::vector<{}>".format(cxx_type(value[:-6]))
    return value


cxx_args = functools.partial(generic_args, type_mapper=cxx_type)
cxx_prototype = functools.partial(generic_prototype, arg_mapper=cxx_args)


@jinja2.environmentfilter
def cxx_comment(env, value: str, doc: bool, indent: int = 0) -> str:
    start = "/// " if doc else "// "
    newline = "\n" + indent * " " + start
    return start + env.call_filter("wordwrap", value,
                                   [79 - indent - len(start), False, newline])
