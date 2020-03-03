// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2013 Association Prologin <association@prologin.org>
///
// This file has been generated, if you wish to
// modify it in a permanent way, please refer
// to the script file : gen/generator_cxx.rb
//

#include "api.hh"

#include <cassert>
#include <cmath>
#include <cstring>

void test_alert()
{
    assert(false);
}

///
// Called 10K times to test if things work well.
//
void test()
{
    assert(CONST_VAL/4 == 10);
    assert(CONST_DOUBLE == 42.42);

    send_me_42(42);
    send_me_42_and_1337(42, 1337);
    send_me_true(true);
    send_me_tau(6.2831853);
    send_me_13_ints(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);
    assert(returns_42() == 42);
    assert(returns_true() == true);
    assert(std::abs(returns_tau() - 6.2831) < 0.001);

    std::vector<int> r = returns_range(1, 100);
    for (int i = 1; i < 100; ++i)
        assert(r[i - 1] == i);

    r = returns_range(1, 10000);
    for (int i = 1; i < 10000; ++i)
        assert(r[i - 1] == i);

    std::vector<int> v = {1, 3, 2, 4, 5, 7, 6};
    v = returns_sorted(v);
    for (int i = 0; i < 7; ++i)
        assert(v[i] == i + 1);

    assert(returns_sorted(r) == r);

    std::vector<bool> ba = {true, false, false, true, false, false, true, false, false};
    ba = returns_not(ba);
    for (int i = 0; i < 9; ++i)
        assert(ba[i] == (i % 3 != 0));

    struct_with_array s;
    s.field_int = 42;
    for (int i = 0; i < 42; ++i)
    {
        s.field_int_arr.push_back(42);
        simple_struct ss = { 42, true, 42.42 };
        s.field_str_arr.push_back(ss);
    }
    send_me_42s(s);

    send_me_test_enum(VAL1, VAL2);

    std::vector<struct_with_array> l(42);
    for (int i = 0; i < 42; ++i)
    {
        l[i].field_int = 42;
        for (int j = 0; j < 42; ++j)
        {
            l[i].field_int_arr.push_back(42);
            simple_struct ss = { 42, true, 42.42 };
            l[i].field_str_arr.push_back(ss);
        }
    }
    l = send_me_struct_array(l);
    assert(l.size() == 42);
    for (int i = 0; i < 42; ++i)
    {
        assert(l[i].field_int == 42);
        assert(l[i].field_int_arr.size() == 42);
        assert(l[i].field_str_arr.size() == 42);
        for (int j = 0; j < 42; ++j)
        {
            assert(l[i].field_int_arr[j] == 42);
            assert(l[i].field_str_arr[j].field_i == 42);
            assert(l[i].field_str_arr[j].field_bool == true);
            assert(l[i].field_str_arr[j].field_double == 42.42);
        }
    }

    // Test custom operators on structs.
    struct_with_array s2;
    s2.field_int = 42;
    for (int i = 0; i < 42; ++i)
    {
        s2.field_int_arr.push_back(42);
        simple_struct ss = { 42, true, 42.42 };
        s2.field_str_arr.push_back(ss);
    }
    assert(s == s2);
    s2.field_int = 1337;
    assert(s < s2);
    assert(std::hash<struct_with_array>()(s2) != 42);
}
