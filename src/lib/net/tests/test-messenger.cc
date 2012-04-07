#include "../messenger.hh"
#include "../message.hh"

#include <gtest/gtest.h>
#include <cstring>
#include <cstdint>
#include <cstdlib>

using namespace net;

class TestMessenger : public ::testing::Test, public Messenger
{
};

//TestMessenger::~TestMessenger(){}

TEST_F(TestMessenger, CheckFrom)
{
    Message* msg = reinterpret_cast<Message*>(new char[sizeof (Message) + 6]);
    msg->type = MSG_ERR;
    msg->client_id = 42;
    msg->size = 6;
    strcpy(msg->data, "Chiche");

    char* data = nullptr;
    uint32_t data_size = from_msg(*msg, reinterpret_cast<void**>(&data));

    ASSERT_EQ(data_size, (uint32_t) 6);

    //ASSERT_STREQ(data, "Chiche");
    ASSERT_EQ(data[0], 'C');
    ASSERT_EQ(data[1], 'h');
    ASSERT_EQ(data[2], 'i');
    ASSERT_EQ(data[3], 'c');
    ASSERT_EQ(data[4], 'h');
    ASSERT_EQ(data[5], 'e');

    delete data;
}

TEST_F(TestMessenger, CheckTo)
{
    const char data[7] = "Chiche";

    Message* msg = to_msg(data, 6);

    ASSERT_EQ(msg->size, (uint32_t) 6);

    //ASSERT_STREQ(msg->data, "Chiche");
    ASSERT_EQ(msg->data[0], 'C');
    ASSERT_EQ(msg->data[1], 'h');
    ASSERT_EQ(msg->data[2], 'i');
    ASSERT_EQ(msg->data[3], 'c');
    ASSERT_EQ(msg->data[4], 'h');
    ASSERT_EQ(msg->data[5], 'e');

    delete msg;
}
