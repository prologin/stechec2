// This file has been generated, if you wish to
// modify it in a permanent way, please refer
// to the script file : gen/generator_cs.rb

using System;
using System.Diagnostics;

namespace Champion {

  class Champion {
    public void Assert(bool val)
    {
      if (!val)
      {
        throw new SystemException("Assertion failed");
      }
    }

    public void TestAlert()
    {
      Assert(false);
    }

    public void Test()
    {
      Api.SendMe42(42);
      Api.SendMe42And1337(42, 1337);
      Api.SendMeTrue(true);
      Api.SendMeTau(6.2831853);
      Api.SendMeJosephMarchand("Joseph Marchand");
      Api.SendMe13Ints(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);

      Assert(Api.Returns42() == 42);
      Assert(Api.ReturnsTrue() == true);
      Assert(Math.Abs(Api.ReturnsTau() - 6.2831853) < 0.0001);
      Assert(Api.ReturnsJosephMarchand() == "Joseph Marchand");
      Assert(Api.ReturnsVal1() == TestEnum.VAL1);

      int[] r = Api.ReturnsRange(1, 100);
      for (int i = 1; i < 100; ++i)
        Assert(r[i - 1] == i);

      r = Api.ReturnsRange(1, 10000);
      for (int i = 1; i < 10000; ++i)
        Assert(r[i - 1] == i);

      r = Api.ReturnsSorted(r);
      for (int i = 1; i < 10000; ++i)
        Assert(r[i - 1] == i);

      int[] v = {1, 3, 2, 4, 5, 7, 6};
      v = Api.ReturnsSorted(v);
      for (int i = 0; i < 7; ++i)
        Assert(v[i] == i + 1);

      bool[] ba = {true, false, false, true, false, false, true, false, false};
      ba = Api.ReturnsNot(ba);
      for (int i = 0; i < 9; ++i) {
        Assert(ba[i] == (i % 3 != 0));
      }

      double[] bdo = {-0.5, 1.0, 12.5, 42.0};
      double[] bdi = Api.ReturnsInverse(bdo);
      for (int i = 0; i < 4; ++i) {
        Assert(bdi[i] - (1 / bdo[i]) < 0.0001);
      }

      TestEnum[] eao = {TestEnum.VAL1, TestEnum.VAL2, TestEnum.VAL2};
      TestEnum[] ear = Api.ReturnsReversedEnums(eao);
      for (int i = 0; i < 3; ++i) {
        Assert(ear[i] == eao[2 - i]);
      }

      string[] sa = {"Alea", "Jacta", "Est"};
      string[] sau = Api.ReturnsUpper(sa);
      Assert(sau[0] == "ALEA");
      Assert(sau[1] == "JACTA");
      Assert(sau[2] == "EST");

      SimpleStruct simple = new SimpleStruct();
      simple.FieldI = 42;
      simple.FieldBool = true;
      simple.FieldDouble = 42.42;
      simple.FieldString = "TTY";
      Api.SendMeSimple(simple);

      SimpleTuple tuple_struct = new SimpleTuple();
      tuple_struct.Field0 = 42;
      tuple_struct.Field1 = true;
      Api.SendMeTupleStruct(tuple_struct);

      StructWithArray s = new StructWithArray();
      s.FieldInt = 42;
      s.FieldIntArr = new int[42];
      s.FieldStrArr = new SimpleStruct[42];
      s.FieldTupArr = new SimpleTuple[42];
      for (int i = 0; i < 42; ++i)
      {
        s.FieldIntArr[i] = 42;
        s.FieldStrArr[i] = simple;
        s.FieldTupArr[i] = tuple_struct;
      }
      Api.SendMe42s(s);

      StructWithOnlyDouble float_struct = new StructWithOnlyDouble();
      float_struct.FieldOne = 42.42;
      float_struct.FieldTwo = 42.42;
      Api.SendMeDoubleStruct(float_struct);

      Api.SendMeTestEnum(TestEnum.VAL1, TestEnum.VAL2);

      Api.AfficherTestEnum(TestEnum.VAL2);

      StructWithStruct s_with_struct = new StructWithStruct();
      s_with_struct.FieldInteger = 42;
      s_with_struct.FieldStruct = simple;
      s_with_struct.FieldTuple = tuple_struct;
      Api.SendMeStructWithStruct(s_with_struct);

      TupleWithStruct t_with_struct = new TupleWithStruct();
      t_with_struct.Field0Int = 42;
      t_with_struct.Field1Struct = simple;
      t_with_struct.Field2Tuple = tuple_struct;
      Api.SendMeTupleWithStruct(t_with_struct);

      StructWithArray[] l = new StructWithArray[42];
      for (int i = 0; i < 42; ++i)
      {
        l[i] = new StructWithArray();
        l[i].FieldInt = 42;
        l[i].FieldIntArr = new int[42];
        l[i].FieldStrArr = new SimpleStruct[42];
        l[i].FieldTupArr = new SimpleTuple[42];
        for (int j = 0; j < 42; ++j)
        {
          l[i].FieldIntArr[j] = 42;
          l[i].FieldStrArr[j] = simple;
          l[i].FieldTupArr[j] = tuple_struct;
        }
      }

      l = Api.SendMeStructArray(l);

      Assert(l.Length == 42);
      for (int i = 0; i < 42; ++i)
      {
        Assert(l[i].FieldInt == 42);
        Assert(l[i].FieldIntArr.Length == 42);
        Assert(l[i].FieldStrArr.Length == 42);
        Assert(l[i].FieldTupArr.Length == 42);
        for (int j = 0; j < 42; ++j)
        {
          Assert(l[i].FieldIntArr[j] == 42);
          Assert(l[i].FieldStrArr[j].FieldI == 42);
          Assert(l[i].FieldStrArr[j].FieldBool == true);
          Assert(l[i].FieldStrArr[j].FieldDouble == 42.42);
          Assert(l[i].FieldStrArr[j].FieldString == "TTY");
          Assert(l[i].FieldTupArr[j].Field0 == 42);
          Assert(l[i].FieldTupArr[j].Field1);
        }
      }

      TupleWithArray t_with_array = new TupleWithArray();
      t_with_array.Field0Count = 42;
      t_with_array.Field1Array = new SimpleTuple[42];
      for (int i = 0; i < 42; ++i)
      {
        t_with_array.Field1Array[i] = tuple_struct;
      }
      Api.SendMeTupleWithArray(t_with_array);
    }
  }
}
