// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#include "../buffer.hh"

#include <gtest/gtest.h>

using namespace utils;

// Tests the buffer class.

struct MyStruct
{
    int x;
    std::string foo;
    char arr[3];

    void handle_buffer(Buffer& buf)
    {
        buf.handle(x);
        buf.handle(foo);
        buf.handle_array(arr, 3);
    }
};

TEST(UtilsBuffer, Serialize)
{
    Buffer buf;

    MyStruct s = {42, "test", {1, 2, 3}};
    s.handle_buffer(buf);

    // Check if the size is as expected
    EXPECT_EQ(15u, buf.size());

    // Check if the contents are as expected
    char contents[16] = "\x2A\x00\x00\x00" // int x
                        "\x04\x00\x00\x00" // foo.size()
                        "\x74\x65\x73\x74" // "test"
                        "\x01\x02\x03";    // arr

    for (int i = 0; i < 15; ++i)
        EXPECT_EQ(contents[i], buf.data()[i]);
}

TEST(UtilsBuffer, Deserialize)
{
    char contents[16] = "\x2A\x00\x00\x00" // int x
                        "\x04\x00\x00\x00" // foo.size()
                        "\x74\x65\x73\x74" // "test"
                        "\x01\x02\x03";    // arr
    std::vector<uint8_t> v;
    v.assign(contents, contents + 15);

    Buffer buf(v);
    MyStruct s;
    s.handle_buffer(buf);

    EXPECT_EQ(42, s.x);
    EXPECT_EQ("test", s.foo);
    EXPECT_EQ(1, s.arr[0]);
    EXPECT_EQ(2, s.arr[1]);
    EXPECT_EQ(3, s.arr[2]);
}

TEST(UtilsBuffer, DeserializeError)
{
    char contents[4] = "foo";
    std::vector<uint8_t> v;
    v.assign(contents, contents + 4);

    Buffer buf(v);
    MyStruct s;
    ASSERT_THROW(s.handle_buffer(buf), DeserializationError);
}
