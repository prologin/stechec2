///
// This file has been generated, if you wish to
// modify it in a permanent way, please refer
// to the script file : gen/generator_c.rb
//

#include "prologin.h"

#include <assert.h>

///
// Called 10K times to test if things work well.
//
void test()
{
    send_me_42(42);
    send_me_42_and_1337(42, 1337);
    send_me_true(true);
    assert(returns_42() == 42);
    assert(returns_true() == true);

    int_array r = returns_range(1, 100);
    for (int i = 1; i < 100; ++i)
        assert(r.datas[i - 1] == i);
    free(r.datas);

    r = returns_range(1, 10000);
    for (int i = 1; i < 10000; ++i)
        assert(r.datas[i - 1] == i);
    free(r.datas);

    int vr[] = {1, 3, 2, 4, 5, 7, 6};
    int_array v = (int_array){vr, 7};
    v = returns_sorted(v);
    for (int i = 0; i < 7; ++i)
        assert(v.datas[i] == i + 1);
    free(v.datas);

    struct_with_array s;
    s.field_int = 42;
    simple_struct ss[42];
    int sr[42];
    for (int i = 0; i < 42; ++i)
    {
        sr[i] = 42;
        ss[i] = (simple_struct){42, true};
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
        l[i].field_int_arr.datas = malloc(42 * sizeof (int));
        l[i].field_str_arr.datas = malloc(42 * sizeof (simple_struct));
        for (int j = 0; j < 42; ++j)
        {
            l[i].field_int_arr.datas[j] = 42;
            l[i].field_str_arr.datas[j] = (simple_struct){ 42, true };
        }
    }
    struct_with_array_array ll = send_me_struct_array((struct_with_array_array){l, 42});
    for (int i = 0; i < 42; ++i)
    {
        free(l[i].field_int_arr.datas);
        free(l[i].field_str_arr.datas);
    }
    assert(ll.length == 42);
    for (int i = 0; i < 42; ++i)
    {
        assert(ll.datas[i].field_int == 42);
        assert(ll.datas[i].field_int_arr.length == 42);
        assert(ll.datas[i].field_str_arr.length == 42);
        for (int j = 0; j < 42; ++j)
        {
            assert(ll.datas[i].field_int_arr.datas[j] == 42);
            assert(ll.datas[i].field_str_arr.datas[j].field_i == 42);
            assert(ll.datas[i].field_str_arr.datas[j].field_bool == true);
        }
    }
    for (int i = 0; i < 42; ++i)
    {
        free(ll.datas[i].field_int_arr.datas);
        free(ll.datas[i].field_str_arr.datas);
    }
    free(ll.datas);
}

