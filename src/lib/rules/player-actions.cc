#include "player-actions.hh"

void PlayerActions::handle_buffer(utils::Buffer& buf)
{
    buf.handle(player_->id);
    buf.handle(player_->score);

    if (buf.serialize())
    {
        for (int i = actions_.size(); i > 0; --i)
        {
            IAction_sptr action(actions_.front());
            action->handle_buffer(buf);
            actions_.pop_front();
        }
    }
    else
    {
        while (buf.size() > 0)
        {
            // get the action id
            uint32_t action_id;
            buf.handle(action_id);

            // Use it to instantiate the right rules Action
            IAction_sptr action = IAction_sptr(action_factory_[action_id]());

            // And finally unserialize
            action->handle_buffer(buf);
            actions_.push_front(action);
        }
    }
}

void PlayerActions::register_action(uint32_t action_id,
        f_action_factory action_factory)
{
    action_factory_[action_id] = action_factory;
}
