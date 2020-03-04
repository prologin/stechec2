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

struct MyBufferizable : IBufferizable
{
    int x;

    void handle_buffer(Buffer& buf) override
    {
        buf.handle(x);
    };
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
    EXPECT_TRUE(buf.empty());
}

TEST(UtilsBuffer, SerializeBuf)
{
    Buffer container;
    Buffer contained;

    int val = 42;
    contained.handle(val);
    container.handle(contained);

    EXPECT_EQ(sizeof(size_t) + sizeof(val), container.size());

    container.handle(contained);

    EXPECT_EQ((sizeof(size_t) + sizeof(val)) * 2, container.size());
}

TEST(UtilsBuffer, DeserializeBuf)
{
    char content[] = "\x04\x00\x00\x00\x00\x00\x00\x00" // size_t buf_size1
                     "\x2a\x00\x00\x00"                 // int val1 = 42
                     "\x04\x00\x00\x00\x00\x00\x00\x00" // size_t buf_size2
                     "\x39\x05\x00\x00";                // int val2 = 1337
    std::vector<uint8_t> v;
    v.assign(content, content + sizeof(content) - 1);

    // Init from vector
    Buffer container{v};

    // Deserialize first buffer
    Buffer buf_ser;
    container.handle(buf_ser);

    // Deserialize value of first buffer
    Buffer buf_des{std::move(buf_ser)};
    int val_1;
    buf_des.handle(val_1);

    EXPECT_TRUE(buf_des.empty());
    EXPECT_EQ(42, val_1);

    // Deserialize second buffer
    buf_ser = Buffer{};
    container.handle(buf_ser);
    buf_des = Buffer{std::move(buf_ser)};

    // Deserialize value of second buffer
    int val_2;
    buf_des.handle(val_2);
    EXPECT_TRUE(buf_des.empty());
    EXPECT_EQ(1337, val_2);

    EXPECT_TRUE(container.empty());
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

TEST(UtilsBuffer, IBufferizable)
{
    MyBufferizable obj;
    obj.x = 42;
    Buffer buf_ser;

    buf_ser.handle_bufferizable(&obj);

    EXPECT_EQ(buf_ser.data()[0], sizeof(obj.x));

    MyBufferizable obj2;
    Buffer buf_des{std::move(buf_ser)};
    buf_des.handle_bufferizable(&obj2);
    EXPECT_EQ(obj2.x, 42);
}
