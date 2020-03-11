// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2013-2020 Association Prologin <association@prologin.org>
// Copyright (c) 2013 Benoît Zanotti
// Copyright (c) 2014-2020 Antoine Pietri
// Copyright (c) 2016 Li-yao Xia
// Copyright (c) 2020 Sacha Delanoue

public class Champion extends Api
{
    public void test_alert()
    {
        assert(false);
    }

    public void test()
    {
        send_me_42(42);
        send_me_42_and_1337(42, 1337);
        send_me_true(true);
        send_me_tau(6.2831853);
        send_me_13_ints(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);
        assert(returns_42() == 42);
        assert(returns_true() == true);
        assert(Math.abs(returns_tau() - 6.2831) < 0.001);

        int[] r = returns_range(1, 100);
        for (int i = 1; i < 100; ++i)
            assert(r[i - 1] == i);

        r = returns_range(1, 10000);
        for (int i = 1; i < 10000; ++i)
            assert(r[i - 1] == i);

        r = returns_sorted(r);
        for (int i = 1; i < 10000; ++i)
            assert(r[i - 1] == i);

        int[] v = {1, 3, 2, 4, 5, 7, 6};
        v = returns_sorted(v);
        for (int i = 0; i < 7; ++i)
            assert(v[i] == i + 1);

        boolean[] ba = {true, false, false, true, false, false, true, false, false};
        ba = returns_not(ba);
        for (int i = 0; i < 9; ++i)
            assert(ba[i] == (i % 3 != 0));

        StructWithArray s = new StructWithArray();
        s.field_int = 42;
        s.field_int_arr = new int[42];
        s.field_str_arr = new SimpleStruct[42];
        for (int i = 0; i < 42; ++i)
        {
            s.field_int_arr[i] = 42;
            SimpleStruct ss = new SimpleStruct();
            ss.field_i = 42;
            ss.field_bool = true;
            ss.field_double = 42.42;
            s.field_str_arr[i] = ss;
        }
        send_me_42s(s);

        send_me_test_enum(TestEnum.VAL1, TestEnum.VAL2);

        afficher_test_enum(TestEnum.VAL2);

        StructWithArray[] l = new StructWithArray[42];
        for (int i = 0; i < 42; ++i)
        {
            l[i] = new StructWithArray();
            l[i].field_int = 42;
            l[i].field_int_arr = new int[42];
            l[i].field_str_arr = new SimpleStruct[42];
            for (int j = 0; j < 42; ++j)
            {
                l[i].field_int_arr[j] = 42;
                SimpleStruct ss = new SimpleStruct();
                ss.field_i = 42;
                ss.field_bool = true;
                ss.field_double = 42.42;
                l[i].field_str_arr[j] = ss;
            }
        }

        l = send_me_struct_array(l);
        assert(l.length == 42);
        for (int i = 0; i < 42; ++i)
        {
            assert(l[i].field_int == 42);
            assert(l[i].field_int_arr.length == 42);
            assert(l[i].field_str_arr.length == 42);
            for (int j = 0; j < 42; ++j)
            {
                assert(l[i].field_int_arr[j] == 42);
                assert(l[i].field_str_arr[j].field_i == 42);
                assert(l[i].field_str_arr[j].field_bool == true);
                assert(l[i].field_str_arr[j].field_double == 42.42);
            }
        }

        // Specific Java test
        StructWithStruct sws = new StructWithStruct();
        assert(sws.field_struct != null);
    }
}
