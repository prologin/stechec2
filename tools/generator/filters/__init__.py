# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (c) 2020 Association Prologin <association@prologin.org>
# Copyright (c) 2020 Antoine Pietri

import functools

FILTER_LIBRARY = {}
FUNCTION_LIBRARY = {}
TEST_LIBRARY = {}


def _register(library, func=None, *, name=None):
    if func is None:
        return functools.partial(register_filter, name=name)
    library[name if name is not None else func.__name__] = func
    return func


def register_filter(func=None, *, name=None):
    return _register(FILTER_LIBRARY, func, name=name)


def register_function(func=None, *, name=None):
    return _register(FUNCTION_LIBRARY, func, name=name)


def register_test(func=None, *, name=None):
    if name is None and func.__name__.startswith('is_'):
        name = func.__name__[len('is_'):]
    return _register(TEST_LIBRARY, func, name=name)


def load_library_in(env):
    from . import (  # noqa: E402, F401
        common,
        c,
        caml,
        cxx,
        haskell,
        java,
        php,
        python,
        rust,
    )
    for filter_name, filter in FILTER_LIBRARY.items():
        env.filters[filter_name] = filter
    for test_name, test in TEST_LIBRARY.items():
        env.tests[test_name] = test
    for function_name, function in FUNCTION_LIBRARY.items():
        env.globals[function_name] = function
