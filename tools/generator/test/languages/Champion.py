# -*- coding: iso-8859-1 -*-
# This file has been generated, if you wish to
# modify it in a permanent way, please refer
# to the script file : gen/generator_python.rb

from api import *


def test_alert():
    assert False


# Called 10K times to test if things work well.
def test():
    send_me_42(42)
    send_me_42_and_1337(42, 1337)
    send_me_true(True)
    send_me_tau(6.2831853)
    send_me_13_ints(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13)
    assert returns_42() == 42
    assert returns_true() == True
    assert abs(returns_tau() - 6.2831853) < 0.0001
    assert returns_val1() == test_enum.VAL1
    assert returns_val1() != test_enum.VAL2
    assert returns_range(1, 100) == list(range(1, 100))
    assert returns_range(1, 10000) == list(range(1, 10000))
    assert returns_sorted([1, 3, 2, 4, 5, 7, 6]) == [1, 2, 3, 4, 5, 6, 7]
    assert returns_sorted(list(range(10000))) == list(range(10000))
    send_me_42s(struct_with_array(
        field_int=42,
        field_int_arr=[42] * 42,
        field_str_arr=[simple_struct(42, True, 42.42)] * 42))

    send_me_test_enum(test_enum.VAL1, test_enum.VAL2)
    l = send_me_struct_array([struct_with_array(
        field_int=42,
        field_int_arr=[42] * 42,
        field_str_arr=[simple_struct(42, True, 42.42)] * 42)] * 42)

    assert l == [struct_with_array(
        field_int=42,
        field_int_arr=[42] * 42,
        field_str_arr=[simple_struct(42, True, 42.42)] * 42)] * 42


def double_me(values):
    return [x * 2 for x in values]
