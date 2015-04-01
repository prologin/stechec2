#include "action-play.hh"

#include <utils/log.hh>

#include "constant.hh"

ActionPlay::ActionPlay(int x, int y, int player)
    : x_(x),
      y_(y),
      player_(player),
      id_(0)
{
}

int ActionPlay::check(const GameState* st) const
{
    DEBUG("ActionPlay::check");

    CHECK(player_ >= 0);

    if (!st->is_valid_cell(x_, y_))
        return OUT_OF_BOUNDS;
    else if (st->cell(x_, y_) != st->NO_PLAYER)
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
    st->cell(x_, y_) = player_;
}
