// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2013 Association Prologin <association@prologin.org>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cctype>
#include <iostream>
#include <sstream>

#include "cxx/api.hh"

struct {
    int send_me_42;
    int send_me_42_and_1337;
    int send_me_true;
    int send_me_tau;
    int send_me_joseph_marchand;
    int send_me_13_ints;
    int send_me_simple;
    int send_me_42s;
    int send_me_double_struct;
    int send_me_test_enum;
    int send_me_struct_with_struct;
    int send_me_struct_array;
    int send_me_tuple_struct;
    int send_me_tuple_with_struct;
    int afficher_test_enum;
    int returns_42;
    int returns_true;
    int returns_tau;
    int returns_joseph_marchand;
    int returns_val1;
    int returns_range; // Test arrays of ints (return value)
    int returns_sorted; // Test arrays of ints
    int returns_not; // Test arrays of bools
    int returns_inverse; // Test arrays of double
    int returns_reversed_enums; // Test arrays of enums
    int returns_upper; // Test arrays of strings
} func_called;

extern "C"
{
    void test();
    void test_alert();

    void api_send_me_42(int n)
    {
        assert(n == 42);
        func_called.send_me_42 += 1;
    }

    void api_send_me_42_and_1337(int n1, int n2)
    {
        assert(n1 == 42);
        assert(n2 == 1337);
        func_called.send_me_42_and_1337 += 1;
    }

    void api_send_me_true(bool b)
    {
        assert(b);
        func_called.send_me_true += 1;
    }

    void api_send_me_tau(double b)
    {
        assert(b > 6.2831 && b < 6.2832);
        func_called.send_me_tau += 1;
    }

    void api_send_me_joseph_marchand(const std::string& s)
    {
        assert(s == "Joseph Marchand");
        func_called.send_me_joseph_marchand += 1;
    }

    void api_send_me_13_ints(int arg1, int arg2, int arg3, int arg4, int arg5,
                             int arg6, int arg7, int arg8, int arg9, int arg10,
                             int arg11, int arg12, int arg13)
    {
        assert(arg1 == 1);
        assert(arg2 == 2);
        assert(arg3 == 3);
        assert(arg4 == 4);
        assert(arg5 == 5);
        assert(arg6 == 6);
        assert(arg7 == 7);
        assert(arg8 == 8);
        assert(arg9 == 9);
        assert(arg10 == 10);
        assert(arg11 == 11);
        assert(arg12 == 12);
        assert(arg13 == 13);
        func_called.send_me_13_ints += 1;
    }

    int api_returns_42()
    {
        func_called.returns_42 += 1;
        return 42;
    }

    bool api_returns_true()
    {
        func_called.returns_true += 1;
        return true;
    }

    double api_returns_tau()
    {
        func_called.returns_tau += 1;
        return 6.2831853;
    }

    const std::string api_returns_joseph_marchand()
    {
        func_called.returns_joseph_marchand += 1;
        return "Joseph Marchand";
    }

    test_enum api_returns_val1()
    {
        func_called.returns_val1 += 1;
        return VAL1;
    }

    std::vector<int> api_returns_range(int m, int n)
    {
        func_called.returns_range += 1;
        std::vector<int> v(n - m);
        for (int i = m; i < n; ++i)
            v[i - m] = i;
        return v;
    }

    std::vector<int> api_returns_sorted(std::vector<int> l)
    {
        func_called.returns_sorted += 1;
        std::sort(l.begin(), l.end());
        return l;
    }

    std::vector<bool> api_returns_not(std::vector<bool> l)
    {
        func_called.returns_not += 1;
        for (unsigned i = 0; i < l.size(); ++i)
            l[i] = !l[i];
        return l;
    }

    std::vector<double> api_returns_inverse(std::vector<double> l)
    {
        func_called.returns_inverse += 1;
        for (unsigned i = 0; i < l.size(); ++i)
            l[i] = 1 / l[i];
        return l;
    }

    std::vector<test_enum> api_returns_reversed_enums(std::vector<test_enum> l)
    {
        func_called.returns_reversed_enums += 1;
        std::reverse(l.begin(), l.end());
        return l;
    }

    std::vector<std::string> api_returns_upper(std::vector<std::string> l)
    {
        func_called.returns_upper += 1;
        for (unsigned i = 0; i < l.size(); ++i)
            std::transform(l[i].begin(), l[i].end(), l[i].begin(),
                           ::toupper);
        return l;
    }

    void api_send_me_simple(simple_struct s)
    {
        assert(s.field_i == 42);
        assert(s.field_bool);
        assert(s.field_double > 42.41 and s.field_double < 42.43);
        assert(s.field_string == "TTY");
        func_called.send_me_simple += 1;
    }

    void api_send_me_tuple_struct(simple_tuple s) {
        assert(s.field_0 == 42);
        assert(s.field_1);
        func_called.send_me_tuple_struct += 1;
    }

    void api_send_me_42s(struct_with_array s)
    {
        assert(s.field_int == 42);
        assert(s.field_int_arr.size() == 42);
        for (auto i : s.field_int_arr)
            assert(i == 42);
        assert(s.field_str_arr.size() == 42);
        for (auto& s2 : s.field_str_arr)
            api_send_me_simple(s2);
        for (auto& t : s.field_tup_arr)
            api_send_me_tuple_struct(t);
        func_called.send_me_42s += 1;
    }

    void api_send_me_double_struct(struct_with_only_double s)
    {
        assert(s.field_one > 42.41 and s.field_one < 42.43);
        assert(s.field_two > 42.41 and s.field_two < 42.43);
        func_called.send_me_double_struct += 1;
    }

    void api_send_me_test_enum(test_enum v1, test_enum v2)
    {
        assert(v1 == VAL1);
        assert(v2 == VAL2);
        func_called.send_me_test_enum += 1;
    }

    void api_send_me_struct_with_struct(struct_with_struct s) {
        assert(s.field_integer == 42);
        assert(s.field_struct.field_i == 42);
        assert(s.field_struct.field_bool);
        assert(s.field_struct.field_double > 42.41 and s.field_struct.field_double < 42.43);
        assert(s.field_struct.field_string == "TTY");
        assert(s.field_tuple.field_0 == 42);
        assert(s.field_tuple.field_1);
        func_called.send_me_struct_with_struct += 1;
    }

    void api_send_me_tuple_with_struct(tuple_with_struct s) {
        assert(s.field_0_int == 42);
        assert(s.field_1_struct.field_i == 42);
        assert(s.field_1_struct.field_bool);
        assert(s.field_1_struct.field_double > 42.41 and s.field_1_struct.field_double < 42.43);
        assert(s.field_1_struct.field_string == "TTY");
        assert(s.field_2_tuple.field_0 == 42);
        assert(s.field_2_tuple.field_1);
        func_called.send_me_tuple_with_struct += 1;
    }

    std::vector<struct_with_array>
    api_send_me_struct_array(std::vector<struct_with_array> l)
    {
        assert(l.size() == 42);
        for (auto& s : l)
            api_send_me_42s(s);
        func_called.send_me_struct_array += 1;
        return l;
    }

    void api_afficher_test_enum(test_enum v)
    {
        assert(v == VAL2);
        func_called.afficher_test_enum += 1;
    }

    void api_afficher_simple_struct(simple_struct) {}
    void api_afficher_struct_with_array(struct_with_array) {}
    void api_afficher_struct_with_struct(struct_with_struct) {}
    void api_afficher_struct_with_only_double(struct_with_only_double) {}
    void api_afficher_simple_tuple(simple_tuple) {}
    void api_afficher_tuple_with_struct(tuple_with_struct) {}
}


int main(int argc, char* argv[])
{
    int count = 100;
    if (argc > 1)
    {
        if (std::string(argv[1]) == "--test-alert")
        {
            test_alert();
            return 0;
        }
        count = std::strtol(argv[1], nullptr, 0);
    }
    for (int i = 0; i < count; ++i)
        test();

    assert(func_called.send_me_42);
    assert(func_called.send_me_42_and_1337);
    assert(func_called.send_me_true);
    assert(func_called.send_me_tau);
    assert(func_called.send_me_joseph_marchand);
    assert(func_called.send_me_13_ints);
    assert(func_called.send_me_simple);
    assert(func_called.send_me_42s);
    assert(func_called.send_me_double_struct);
    assert(func_called.send_me_test_enum);
    assert(func_called.send_me_struct_with_struct);
    assert(func_called.send_me_struct_array);
    assert(func_called.send_me_tuple_struct);
    assert(func_called.send_me_tuple_with_struct);
    assert(func_called.afficher_test_enum);
    assert(func_called.returns_42);
    assert(func_called.returns_true);
    assert(func_called.returns_tau);
    assert(func_called.returns_joseph_marchand);
    assert(func_called.returns_val1);
    assert(func_called.returns_range);
    assert(func_called.returns_sorted);
    assert(func_called.returns_not);
    assert(func_called.returns_inverse);
    assert(func_called.returns_reversed_enums);
    assert(func_called.returns_upper);
}
