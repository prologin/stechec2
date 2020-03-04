#include "../game-state-history.hh"
#include "../game-state.hh"

#include "gtest/gtest.h"

using rules::GameStateHistory;

class State final : public rules::GameState
{
public:
    State(int a)
        : a(a)
    {
    }
    State* copy() const
    {
        return new State(*this);
    }
    int a;
};

TEST(GameStateHistory, save_cancel)
{
    auto v = GameStateHistory<State>(std::make_unique<State>(42));

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

TEST(GameStateHistory, clear)
{
    auto v = GameStateHistory<State>(std::make_unique<State>(42));
    v.save();
    v.save();
    v.save();
    v.clear_old_versions();
    EXPECT_EQ(v->a, 42);
    EXPECT_FALSE(v.can_cancel());
}

TEST(GameStateHistory, multiple_cancel)
{
    auto v = GameStateHistory<State>(std::make_unique<State>(42));
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
