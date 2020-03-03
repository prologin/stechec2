///
// This file has been generated, if you wish to
// modify it in a permanent way, please refer
// to the script file : gen/generator_c.rb
//

#include "api.h"

#include <assert.h>
#include <math.h>

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
    assert(fabs(returns_tau() - 6.2831) < 0.001);

    int_array r = returns_range(1, 100);
    for (int i = 1; i < 100; ++i)
        assert(r.items[i - 1] == i);
    free(r.items);

    r = returns_range(1, 10000);
    for (int i = 1; i < 10000; ++i)
        assert(r.items[i - 1] == i);
    free(r.items);

    int vr[] = {1, 3, 2, 4, 5, 7, 6};
    int_array v = (int_array){vr, 7};
    v = returns_sorted(v);
    for (int i = 0; i < 7; ++i)
        assert(v.items[i] == i + 1);
    free(v.items);

    struct_with_array s;
    s.field_int = 42;
    simple_struct ss[42];
    int sr[42];
    for (int i = 0; i < 42; ++i)
    {
        sr[i] = 42;
        ss[i] = (simple_struct){42, true, 42.42};
    }
    s.field_int_arr = (int_array){sr, 42};
    s.field_str_arr = (simple_struct_array){ss, 42};
    send_me_42s(s);

    send_me_test_enum(VAL1, VAL2);

    struct_with_array l[42];
    for (int i = 0; i < 42; ++i)
    {
        l[i].field_int = 42;
        l[i].field_int_arr.length = 42;
        l[i].field_str_arr.length = 42;
        l[i].field_int_arr.items = malloc(42 * sizeof (int));
        l[i].field_str_arr.items = malloc(42 * sizeof (simple_struct));
        for (int j = 0; j < 42; ++j)
        {
            l[i].field_int_arr.items[j] = 42;
            l[i].field_str_arr.items[j] = (simple_struct){ 42, true, 42.42 };
        }
    }
    struct_with_array_array ll = send_me_struct_array((struct_with_array_array){l, 42});
    for (int i = 0; i < 42; ++i)
    {
        free(l[i].field_int_arr.items);
        free(l[i].field_str_arr.items);
    }
    assert(ll.length == 42);
    for (int i = 0; i < 42; ++i)
    {
        assert(ll.items[i].field_int == 42);
        assert(ll.items[i].field_int_arr.length == 42);
        assert(ll.items[i].field_str_arr.length == 42);
        for (int j = 0; j < 42; ++j)
        {
            assert(ll.items[i].field_int_arr.items[j] == 42);
            assert(ll.items[i].field_str_arr.items[j].field_i == 42);
            assert(ll.items[i].field_str_arr.items[j].field_bool == true);
            assert(ll.items[i].field_str_arr.items[j].field_double == 42.42);
        }
    }
    for (int i = 0; i < 42; ++i)
    {
        free(ll.items[i].field_int_arr.items);
        free(ll.items[i].field_str_arr.items);
    }
    free(ll.items);
}


int_array double_me(int_array values)
{
    for (int i = 0 ; i < values.length ; i++)
        values.items[i] *= 2;

    return values;
}
