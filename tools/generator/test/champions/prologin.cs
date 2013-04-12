// This file has been generated, if you wish to
// modify it in a permanent way, please refer
// to the script file : gen/generator_cs.rb

using System;
using System.Diagnostics;

namespace Prologin {

  class Prologin {
    public void Test()
    {
      Api.SendMe42(42);
      Api.SendMe42And1337(42, 1337);
      Api.SendMeTrue(true);
      Debug.Assert(Api.Returns42() == 42);
      Debug.Assert(Api.ReturnsTrue() == true);

      int[] r = Api.ReturnsRange(1, 100);
      for (int i = 1; i < 100; ++i)
        Debug.Assert(r[i - 1] == i);

      r = Api.ReturnsRange(1, 10000);
      for (int i = 1; i < 10000; ++i)
        Debug.Assert(r[i - 1] == i);

      r = Api.ReturnsSorted(r);
      for (int i = 1; i < 10000; ++i)
        Debug.Assert(r[i - 1] == i);

      int[] v = {1, 3, 2, 4, 5, 7, 6};
      v = Api.ReturnsSorted(v);
      for (int i = 0; i < 7; ++i)
        Debug.Assert(v[i] == i + 1);

      StructWithArray s = new StructWithArray();
      s.FieldInt = 42;
      s.FieldIntArr = new int[42];
      s.FieldStrArr = new SimpleStruct[42];
      for (int i = 0; i < 42; ++i)
      {
        s.FieldIntArr[i] = 42;
        SimpleStruct ss = new SimpleStruct();
        ss.FieldI = 42;
        ss.FieldBool = true;
        s.FieldStrArr[i] = ss;
      }
      Api.SendMe42s(s);

      Api.SendMeTestEnum(TestEnum.VAL1, TestEnum.VAL2);

      StructWithArray[] l = new StructWithArray[42];
      for (int i = 0; i < 42; ++i)
      {
        l[i] = new StructWithArray();
        l[i].FieldInt = 42;
        l[i].FieldIntArr = new int[42];
        l[i].FieldStrArr = new SimpleStruct[42];
        for (int j = 0; j < 42; ++j)
        {
          l[i].FieldIntArr[j] = 42;
          SimpleStruct ss = new SimpleStruct();
          ss.FieldI = 42;
          ss.FieldBool = true;
          l[i].FieldStrArr[j] = ss;
        }
      }

      l = Api.SendMeStructArray(l);
      Debug.Assert(l.Length == 42);
      for (int i = 0; i < 42; ++i)
      {
        Debug.Assert(l[i].FieldInt == 42);
        Debug.Assert(l[i].FieldIntArr.Length == 42);
        Debug.Assert(l[i].FieldStrArr.Length == 42);
        for (int j = 0; j < 42; ++j)
        {
          Debug.Assert(l[i].FieldIntArr[j] == 42);
          Debug.Assert(l[i].FieldStrArr[j].FieldI == 42);
          Debug.Assert(l[i].FieldStrArr[j].FieldBool == true);
        }
      }
    }
  }
}
