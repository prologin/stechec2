#include "../messenger.hh"
#include "../message.hh"

#include <gtest/gtest.h>
#include <cstring>
#include <cstdint>
#include <cstdlib>

using namespace net;

class TestMessenger : public ::testing::Test, public Messenger
{
public:
    TestMessenger()
        : Messenger(nullptr)
    {}
};

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
    ASSERT_STREQ(data, "Chiche");

    delete data;
}

TEST_F(TestMessenger, CheckTo)
{
    const char data[7] = "Chiche";

    Message* msg = to_msg(data, 6);

    ASSERT_EQ(msg->size, (uint32_t) 6);

    //FILE* f = fopen("/tmp/lol", "w");
    //fwrite(msg->data, 6, 1, f);
    //fclose(f);

    //ASSERT_STREQ(msg->data, "Chiche");

    delete msg;
}
