import jinja2


def cxx_type(value: str) -> str:
    if value == "string":
        return "std::string"
    if value.endswith(" array"):
        return "std::vector<{}>".format(cxx_type(value[:-6]))
    return value


def cxx_args(value) -> str:
    return ", ".join("{} {}".format(cxx_type(type_), name)
                     for [name, type_, _] in value)


@jinja2.environmentfilter
def cxx_comment(env, value: str, doc: bool, indent: int = 0) -> str:
    start = "/// " if doc else "// "
    newline = "\n" + indent * " " + start
    return start + env.call_filter("wordwrap", value,
                                   [79 - indent - len(start), False, newline])
