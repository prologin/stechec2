#include "action.hh"

namespace rules {

GameState* IAction::apply(GameState* st) const
{
    // Copy the old state and link the new version to the old version.
    GameState* new_st = st->copy();
    new_st->set_old_version(st);

    // Apply the action on the new state.
    this->apply_on(new_st);

    return new_st;
}

void PlayerActions::handle_buffer(utils::Buffer& buf)
{
    buf.handle(player->id);
    buf.handle(player->score);

    if (buf.serialize())
    {
        for (int i = actions.size(); i > 0; --i)
        {
            IAction_sptr action(actions.front());
            action->handle_buffer(buf);
            actions.pop_front();
        }
    }
    else
    {
        while (buf.size() > 0)
        {
            IAction_sptr action = IAction_sptr(action_factory());
            action->handle_buffer(buf);
            actions.push_front(action);
        }
    }
}

} // namespace rules
