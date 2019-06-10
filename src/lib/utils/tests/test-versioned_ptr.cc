#include "../versioned_ptr.hh"

#include "gtest/gtest.h"

class State
{
public:
    State(int a) : a(a) {}

    int a;
};

TEST(VersionedPtr, save_cancel)
{
    auto v = utils::VersionedPtr<State>(std::make_unique<State>(42));

    EXPECT_EQ(v->a, 42);

    EXPECT_FALSE(v.can_cancel());
    v.save();
    EXPECT_TRUE(v.can_cancel());

    v->a = 1337;
    EXPECT_EQ(v->a, 1337);

    v.cancel();
    EXPECT_FALSE(v.can_cancel());

    EXPECT_EQ(v->a, 42);
}

TEST(VersionedPtr, clear)
{
    auto v = utils::VersionedPtr<State>(std::make_unique<State>(42));
    v.save();
    v.save();
    v.save();
    v.clear_old_version();
    EXPECT_EQ(v->a, 42);
    EXPECT_FALSE(v.can_cancel());
}

TEST(VersionedPtr, multiple_cancel)
{
    auto v = utils::VersionedPtr<State>(std::make_unique<State>(42));
    v.save();    // a = 42, 1 save
    v->a = 1337; // mutate current
    v.save();    // a = 1337, 2 saves
    v->a = 1234; // mutate current
    v.save();    // a = 1234, 3 saves
    v.cancel();  // a = 1234, 2 save
    EXPECT_EQ(v->a, 1234);
    v.cancel(); // a = 1337, 1 save
    EXPECT_EQ(v->a, 1337);

    // ASAN will verify that no memory is leaked
}
