// This file has been generated, if you wish to
// modify it in a permanent way, please refer
// to the script file : gen/generator_java.rb

public class Prologin extends Interface
{
  // Fonction appellée au début de la partie
  public void test()
  {
    send_me_42(42);
    send_me_42_and_1337(42, 1337);
    send_me_true(true);
    assert(returns_42() == 42);
    assert(returns_true() == true);

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
        s.field_str_arr[i] = ss;
    }
    send_me_42s(s);

    send_me_test_enum(TestEnum.VAL1, TestEnum.VAL2);

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
      }
    }

  }
}
