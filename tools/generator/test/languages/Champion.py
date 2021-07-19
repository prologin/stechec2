from api import *


def test_alert():
    assert False


# Called 10K times to test if things work well.
def test():
    send_me_42(42)
    send_me_42_and_1337(42, 1337)
    send_me_true(True)
    send_me_tau(6.2831853)
    send_me_joseph_marchand("Joseph Marchand")
    send_me_13_ints(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13)
    assert returns_42() == 42
    assert returns_true() == True
    assert abs(returns_tau() - 6.2831853) < 0.0001
    assert returns_joseph_marchand() == "Joseph Marchand"
    assert returns_val1() == test_enum.VAL1
    assert returns_val1() != test_enum.VAL2
    assert returns_range(1, 100) == list(range(1, 100))
    assert returns_range(1, 10000) == list(range(1, 10000))
    assert returns_sorted([1, 3, 2, 4, 5, 7, 6]) == [1, 2, 3, 4, 5, 6, 7]
    assert returns_sorted(list(range(10000))) == list(range(10000))
    assert returns_not([i % 3 == 0 for i in range(9)]) == [i % 3 != 0 for i in range(9)]

    bdo = [-0.5, 1.0, 12.5, 42.0]
    bdi = returns_inverse(bdo)
    for i, _ in enumerate(bdo):
        assert(bdi[i] - (1 / bdo[i]) < 0.0001)

    ea = [test_enum.VAL1, test_enum.VAL2, test_enum.VAL2]
    assert returns_reversed_enums(ea) == list(reversed(ea))

    assert returns_upper(["Alea", "Jacta", "Est"]) == ["ALEA", "JACTA", "EST"]

    simple = simple_struct(
        field_i=42,
        field_bool=True,
        field_double=42.42,
        field_string="TTY",
    )
    simple_tup = ((42, True))

    send_me_simple(simple)
    send_me_42s(struct_with_array(
        field_int=42,
        field_int_arr=[42] * 42,
        field_str_arr=[simple] * 42,
        field_tup_arr=[(42, True)] * 42))

    send_me_double_struct(struct_with_only_double(
        field_one=42.42,
        field_two=42.42))
    
    send_me_tuple_struct(simple_tup)

    send_me_test_enum(test_enum.VAL1, test_enum.VAL2)
    afficher_test_enum(test_enum.VAL2)

    send_me_struct_with_struct(struct_with_struct(
        field_integer=42,
        field_struct=simple,
        field_tuple=simple_tup))
    send_me_tuple_with_struct((42, simple, simple_tup))

    l = send_me_struct_array([struct_with_array(
        field_int=42,
        field_int_arr=[42] * 42,
        field_str_arr=[simple] * 42,
        field_tup_arr=[(42, True)] * 42)] * 42)

    assert l == [struct_with_array(
        field_int=42,
        field_int_arr=[42] * 42,
        field_str_arr=[simple] * 42,
        field_tup_arr=[(42, True)] * 42)] * 42
