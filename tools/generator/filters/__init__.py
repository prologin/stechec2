import functools

FILTER_LIBRARY = {}
FUNCTION_LIBRARY = {}


def register_filter(func=None, *, name=None):
    if func is None:
        return functools.partial(register_filter, name=name)
    FILTER_LIBRARY[name if name is not None else func.__name__] = func
    return func


def register_function(func=None, *, name=None):
    if func is None:
        return functools.partial(register_function, name=name)
    FUNCTION_LIBRARY[name if name is not None else func.__name__] = func
    return func


def load_library_in(env):
    from . import (  # noqa: E402, F401
        common,
        c,
        cxx
    )
    for filter_name, filter in FILTER_LIBRARY.items():
        env.filters[filter_name] = filter
    for function_name, function in FUNCTION_LIBRARY.items():
        env.globals[function_name] = function
