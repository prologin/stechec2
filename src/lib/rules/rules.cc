#include "rules.hh"

namespace rules {

Rules::Rules(const Options opt)
    : opt_(opt),
    players_(opt.players),
    spectators_(opt.spectators),
    timeout_(opt.time)
{
}

bool Rules::is_spectator(uint32_t id)
{
    for (Player_sptr spectator : spectators_->players)
        if (spectator->id == id)
            return true;

    return false;
}

SynchronousRules::SynchronousRules(const Options opt)
    : Rules(opt)
{
}

void SynchronousRules::client_loop(ClientMessenger_sptr msgr)
{
    at_start();
    at_client_start();

    uint32_t last_player_id;
    msgr->pull_id(&last_player_id);

    while (!is_finished())
    {
        uint32_t playing_id;

        /* Other players turns */
        if (msgr->wait_for_turn(opt_.player->id, &playing_id))
        {
            if (is_spectator(playing_id))
                continue;

            /* Get actions of other players */
            Actions* actions = get_actions();
            msgr->pull_actions(actions);

        }
        else /* Current player turn */
        {
            player_turn();
            Actions* actions = get_actions();
            /* We only want to send back the actions from the current player */
            Actions player_actions;
            for (auto action: actions->actions())
                if (action->player_id() == opt_.player->id)
                    player_actions.add(action);
            msgr->send_actions(player_actions);
            msgr->wait_for_ack();
        }

        /* End of each turn */
        if (last_player_id == playing_id)
        {
            /* Apply actions onto the gamestate */
            Actions* actions = get_actions();
            for (auto action : actions->actions())
                apply_action(action);
            actions->clear();
            end_of_turn();
            start_of_turn();
        }
    }

    at_end();
    at_client_end();
}

void SynchronousRules::spectator_loop(ClientMessenger_sptr msgr)
{
    at_start();
    at_spectator_start();

    uint32_t last_player_id;
    msgr->pull_id(&last_player_id);

    while (!is_finished())
    {
        uint32_t playing_id;

        /* Other players turns */
        if (msgr->wait_for_turn(opt_.player->id, &playing_id))
        {
            if (is_spectator(playing_id))
                continue;

            /* Get actions of other players */
            Actions* actions = get_actions();
            msgr->pull_actions(actions);

        }
        else /* Current player turn */
        {
            spectator_turn();
            Actions* actions = get_actions();
            /* We only want to send back the actions from the current player */
            Actions player_actions;
            for (auto action: actions->actions())
                if (action->player_id() == opt_.player->id)
                    player_actions.add(action);
            msgr->send_actions(player_actions);
            msgr->wait_for_ack();
        }

        /* End of each turn */
        if (last_player_id == playing_id)
        {
            /* Apply actions onto the gamestate */
            Actions* actions = get_actions();
            for (auto action : actions->actions())
                apply_action(action);
            actions->clear();
            end_of_turn();
            start_of_turn();
        }
    }

    at_end();
    at_spectator_end();
}

void SynchronousRules::server_loop(ServerMessenger_sptr msgr)
{
    at_start();
    at_server_start();

    while (!is_finished())
    {
        for (unsigned int i = 0; i < players_->players.size(); i++)
        {
            msgr->push_id(players_->players[i]->id);
            if (!msgr->poll(timeout_))
                continue;
            Actions* actions = get_actions();
            msgr->recv_actions(actions);
            msgr->ack();
        }

        for (unsigned int i = 0; i < spectators_->players.size(); i++)
        {
            msgr->push_id(spectators_->players[i]->id);
            Actions* actions = get_actions();
            msgr->recv_actions(actions);
            msgr->ack();
        }

        Actions* actions = get_actions();
        for (auto action: actions->actions())
            apply_action(action);
        msgr->push_actions(*actions);
        end_of_turn();
        start_of_turn();
    }

    at_end();
    at_server_end();
}

TurnBasedRules::TurnBasedRules(const Options opt)
    : Rules(opt)
{
}

void TurnBasedRules::client_loop(ClientMessenger_sptr msgr)
{
    uint32_t last_player_id;
    msgr->pull_id(&last_player_id);

    at_start();
    at_client_start();

    while (!is_finished())
    {
        uint32_t playing_id;

        /* Other players turns */
        if (msgr->wait_for_turn(opt_.player->id, &playing_id))
        {
            if (is_spectator(playing_id))
                continue;

            /* Get current player actions */
            Actions* actions = get_actions();
            actions->clear();
            msgr->pull_actions(actions);

            /* Apply them onto the gamestate */
            for (auto action : actions->actions())
                apply_action(action);
        }
        else /* Current player turn */
        {
            get_actions()->clear();
            player_turn();
            Actions* actions = get_actions();
            msgr->send_actions(*actions);
            msgr->wait_for_ack();
            msgr->pull_actions(actions);
            actions->clear();
        }

        /* End of each move */
        end_of_player_turn(playing_id);

        /* End of each turn */
        if (last_player_id == playing_id)
        {
            end_of_turn();
            start_of_turn();
        }
    }

    at_end();
    at_client_end();
}

void TurnBasedRules::spectator_loop(ClientMessenger_sptr msgr)
{
    uint32_t last_player_id;
    msgr->pull_id(&last_player_id);

    at_start();
    at_spectator_start();

    while (!is_finished())
    {

        uint32_t playing_id;

        /* Other players turns */
        if (msgr->wait_for_turn(opt_.player->id, &playing_id))
        {
            if (is_spectator(playing_id))
                continue;

            /* Get current player actions */
            Actions* actions = get_actions();
            actions->clear();
            msgr->pull_actions(actions);

            /* Apply them onto the gamestate */
            for (auto action : actions->actions())
                apply_action(action);

            /* End of each move */
            end_of_player_turn(playing_id);
        }
        else /* Current player turn */
        {
            get_actions()->clear();
            spectator_turn();
            Actions* actions = get_actions();
            msgr->send_actions(*actions);
            msgr->wait_for_ack();
            msgr->pull_actions(actions);
            actions->clear();
        }
        /* End of each turn */
        if (last_player_id == playing_id)
        {
            end_of_turn();
            start_of_turn();
        }
    }

    at_end();
    at_spectator_end();
}

void TurnBasedRules::server_loop(ServerMessenger_sptr msgr)
{
    msgr->push_id(players_->players[players_->players.size() - 1]->id);

    at_start();
    at_server_start();

    while (!is_finished())
    {
        for (unsigned int i = 0; i < players_->players.size(); i++)
        {
            msgr->push_id(players_->players[i]->id);
            if (!msgr->poll(timeout_))
                continue;

            Actions* actions = get_actions();
            actions->clear();
            msgr->recv_actions(actions);
            msgr->ack();

            for (auto action: actions->actions())
                apply_action(action);

            msgr->push_actions(*actions);

            end_of_player_turn(players_->players[i]->id);
        }

        for (unsigned int i = 0; i < spectators_->players.size(); i++)
        {
            msgr->push_id(spectators_->players[i]->id);
            Actions* actions = get_actions();
            actions->clear();
            msgr->recv_actions(actions);
            msgr->ack();
        }

        end_of_turn();
        start_of_turn();
    }

    at_end();
    at_server_end();
}

} // namespace rules
