import functools

FILTER_LIBRARY = {}


def register_filter(func=None, *, name=None):
    if func is None:
        return functools.partial(register_filter, name=name)

    FILTER_LIBRARY[func.__name__] = func
    return func


def load_library():
    from . import (  # noqa: E402, F401
        common,
        c,
        cxx
    )
    return FILTER_LIBRARY
