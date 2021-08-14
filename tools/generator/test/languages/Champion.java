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
        send_me_joseph_marchand("Joseph Marchand");
        send_me_13_ints(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);
        assert(returns_42() == 42);
        assert(returns_true() == true);
        assert(Math.abs(returns_tau() - 6.2831) < 0.001);
        assert(returns_joseph_marchand().equals("Joseph Marchand"));
        assert(returns_val1() == TestEnum.VAL1);

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

        double[] bdo = {-0.5, 1.0, 12.5, 42.0};
        double[] bdi = returns_inverse(bdo);
        for (int i = 0; i < 4; ++i) {
            assert(bdi[i] - (1 / bdo[i]) < 0.0001);
        }

        TestEnum[] eao = {TestEnum.VAL1, TestEnum.VAL2, TestEnum.VAL2};
        TestEnum[] ear = returns_reversed_enums(eao);
        for (int i = 0; i < 3; ++i) {
            assert(ear[i] == eao[2 - i]);
        }

        String[] sa = {"Alea", "Jacta", "Est"};
        String[] sau = returns_upper(sa);
        assert(sau[0].equals("ALEA"));
        assert(sau[1].equals("JACTA"));
        assert(sau[2].equals("EST"));

        SimpleStruct simple = new SimpleStruct();
        simple.field_i = 42;
        simple.field_bool = true;
        simple.field_double = 42.42;
        simple.field_string = CONST_STR;
        send_me_simple(simple);

        SimpleTuple tuple_struct = new SimpleTuple();
        tuple_struct.field_0 = 42;
        tuple_struct.field_1 = true;
        send_me_tuple_struct(tuple_struct);

        StructWithArray s = new StructWithArray();
        s.field_int = 42;
        s.field_int_arr = new int[42];
        s.field_str_arr = new SimpleStruct[42];
        s.field_tup_arr = new SimpleTuple[42];
        for (int i = 0; i < 42; ++i)
        {
            s.field_int_arr[i] = 42;
            s.field_str_arr[i] = simple;
            s.field_tup_arr[i] = tuple_struct;
        }
        send_me_42s(s);

        StructWithOnlyDouble float_struct = new StructWithOnlyDouble();
        float_struct.field_one = 42.42;
        float_struct.field_two = 42.42;
        send_me_double_struct(float_struct);

        send_me_test_enum(TestEnum.VAL1, TestEnum.VAL2);

        afficher_test_enum(TestEnum.VAL2);

        StructWithStruct s_with_struct = new StructWithStruct();
        s_with_struct.field_integer = 42;
        s_with_struct.field_struct = simple;
        s_with_struct.field_tuple = tuple_struct;
        send_me_struct_with_struct(s_with_struct);

        TupleWithStruct t_with_struct = new TupleWithStruct();
        t_with_struct.field_0_int = 42;
        t_with_struct.field_1_struct = simple;
        t_with_struct.field_2_tuple = tuple_struct;
        send_me_tuple_with_struct(t_with_struct);

        StructWithArray[] l = new StructWithArray[42];
        for (int i = 0; i < 42; ++i)
        {
            l[i] = new StructWithArray();
            l[i].field_int = 42;
            l[i].field_int_arr = new int[42];
            l[i].field_str_arr = new SimpleStruct[42];
            l[i].field_tup_arr = new SimpleTuple[42];
            for (int j = 0; j < 42; ++j)
            {
                l[i].field_int_arr[j] = 42;
                l[i].field_str_arr[j] = simple;
                l[i].field_tup_arr[j] = tuple_struct;
            }
        }

        l = send_me_struct_array(l);
        assert(l.length == 42);
        for (int i = 0; i < 42; ++i)
        {
            assert(l[i].field_int == 42);
            assert(l[i].field_int_arr.length == 42);
            assert(l[i].field_str_arr.length == 42);
            assert(l[i].field_tup_arr.length == 42);
            for (int j = 0; j < 42; ++j)
            {
                assert(l[i].field_int_arr[j] == 42);
                assert(l[i].field_str_arr[j].field_i == 42);
                assert(l[i].field_str_arr[j].field_bool == true);
                assert(l[i].field_str_arr[j].field_double == 42.42);
                assert(l[i].field_str_arr[j].field_string.equals(CONST_STR));
                assert(l[i].field_tup_arr[j].field_0 == 42);
                assert(l[i].field_tup_arr[j].field_1);
            }
        }

        TupleWithArray t_with_array = new TupleWithArray();
        t_with_array.field_0_count = 42;
        t_with_array.field_1_array = new SimpleTuple[42];
        for (int i = 0; i < 42; i++)
        {
            t_with_array.field_1_array[i] = tuple_struct;
        }
        send_me_tuple_with_array(t_with_array);

        // Specific Java test
        StructWithStruct sws = new StructWithStruct();
        assert(sws.field_struct != null);
    }
}
