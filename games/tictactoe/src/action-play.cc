#include "action-play.hh"

#include <utils/log.hh>

#include "constant.hh"

ActionPlay::ActionPlay(int x, int y, int player)
    : x_(x), y_(y), player_(player)
{
}

int ActionPlay::check(const GameState* st) const
{
    DEBUG("ActionPlay::check");

    CHECK(player_ >= 0);

    if (x_ < 0 || x_ > 3)
        return OUT_OF_BOUNDS;
    else if (y_ < 0 || y_ > 3)
        return OUT_OF_BOUNDS;
    else if (st->board()[3 * y_ + x_] != -1)
        return ALREADY_OCCUPIED;

    return OK;
}

void ActionPlay::handle_buffer(utils::Buffer& buf)
{
    buf.handle(x_);
    buf.handle(y_);
    buf.handle(player_);
}

void ActionPlay::apply_on(GameState* st) const
{
    st->board()[3 * y_ + x_] = player_;
}
