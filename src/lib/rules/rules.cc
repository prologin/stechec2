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

 /*-----------------.
 | SynchronousRules |
 `-----------------*/

SynchronousRules::SynchronousRules(const Options opt)
    : Rules(opt)
{
}

void SynchronousRules::client_loop(ClientMessenger_sptr msgr)
{
    uint32_t last_player_id;
    msgr->pull_id(&last_player_id);

    at_start();
    at_client_start();

    start_of_turn();
    while (!is_finished())
    {
        uint32_t playing_id;

        DEBUG("Waiting for a turn...");

        /* Current player turn */
        if (!msgr->wait_for_turn(opt_.player->id, &playing_id))
        {
            DEBUG("Turn for player %d (me!!!)", playing_id);

            Actions* actions = get_actions();
            actions->clear();

            player_turn();

            /* We only want to send back the actions from the current player */
            Actions player_actions;
            for (auto action: actions->actions())
                if (action->player_id() == opt_.player->id)
                    player_actions.add(action);

            DEBUG("Sending %u actions...", player_actions.size());
            msgr->send_actions(player_actions);
            msgr->wait_for_ack();
        }

        /* End of each turn */
        if (last_player_id == playing_id)
        {
            Actions* actions = get_actions();
            actions->clear();

            /* Get actions of other players */
            DEBUG("Getting actions...");
            msgr->pull_actions(actions);
            DEBUG("Got %u actions", actions->size());

            /* Apply actions onto the gamestate */
            /* We should already have applied our actions */
            for (auto action : actions->actions())
                if (action->player_id() != opt_.player->id)
                    apply_action(action);

            DEBUG("End of turn!");
            end_of_turn();
            if (!is_finished())
                start_of_turn();
            else
                break; // Avoid calling is_finished() twice
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

    start_of_turn();
    while (!is_finished())
    {
        uint32_t playing_id;

        /* Current players turn */
        if (!msgr->wait_for_turn(opt_.player->id, &playing_id))
        {
            Actions* actions = get_actions();
            actions->clear();
            spectator_turn();

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
            actions->clear();
            msgr->pull_actions(actions);
            for (auto action : actions->actions())
                apply_action(action);
            actions->clear();

            end_of_turn();
            if (!is_finished())
                start_of_turn();
            else
                break; // Avoid calling is_finished() twice
        }
    }

    at_end();
    at_spectator_end();
}

void SynchronousRules::server_loop(ServerMessenger_sptr msgr)
{
    msgr->push_id(players_->players[players_->players.size() - 1]->id);

    at_start();
    at_server_start();

    start_of_turn();
    while (!is_finished())
    {
        Actions* actions = get_actions();
        actions->clear();

        msgr->poll(timeout_);
        msgr->recv_actions(actions);
        msgr->ack();

        for (auto action: actions->actions())
            apply_action(action);
        msgr->push_actions(*actions);

        for (unsigned int i = 0; i < spectators_->players.size(); i++)
        {
            msgr->push_id(spectators_->players[i]->id);
            Actions* actions = get_actions();
            actions->clear();
            msgr->recv_actions(actions);
            msgr->ack();
            actions->clear();
        }

        end_of_turn();
        if (!is_finished())
            start_of_turn();
        else
            break; // Avoid calling is_finished() twice
    }

    at_end();
    at_server_end();
}


 /*---------------.
 | TurnBasedRules |
 `---------------*/

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

    start_of_turn();
    while (!is_finished())
    {
        uint32_t playing_id;

        DEBUG("Waiting for a turn...");
        /* Other players turns */
        if (msgr->wait_for_turn(opt_.player->id, &playing_id))
        {
            if (is_spectator(playing_id))
            {
                /* Actions of spectators are not published. */
                DEBUG("Turn for spectator %d, never mind...", playing_id);
                continue;
            }

            DEBUG("Turn for player %d (not me)", playing_id);

            /* Get current player actions */
            Actions* actions = get_actions();
            actions->clear();
            DEBUG("Getting actions...");
            msgr->pull_actions(actions);
            DEBUG("Got %u actions", actions->size());

            /* Apply them onto the gamestate */
            for (auto action : actions->actions())
                apply_action(action);
        }
        else /* Current player turn */
        {
            DEBUG("Turn for player %d (me!!!)", playing_id);
            get_actions()->clear();
            player_turn();
            Actions* actions = get_actions();
            DEBUG("Sending %u actions...", actions->size());
            msgr->send_actions(*actions);
            DEBUG("Waiting for acknowledgement...");
            msgr->wait_for_ack();
            DEBUG("Getting actions...");
            msgr->pull_actions(actions);
            DEBUG("Got %u actions", actions->size());
            actions->clear();
        }

        /* End of each move */
        end_of_player_turn(playing_id);

        /* End of each turn */
        if (last_player_id == playing_id)
        {
            DEBUG("End of turn!");
            end_of_turn();
            if (!is_finished())
                start_of_turn();
            else
                break; // Avoid calling is_finished() twice
        }
    }

    at_end();
    at_client_end();
}

void TurnBasedRules::spectator_loop(ClientMessenger_sptr msgr)
{
    bool last_turn = false;
    uint32_t last_player_id;
    msgr->pull_id(&last_player_id);

    at_start();
    at_spectator_start();

    start_of_turn();
    /* `last_turn` allows us to inspect the final state of the game, when no
     * other player can play anymore. */
    while (last_turn || !is_finished())
    {

        uint32_t playing_id;

        DEBUG("Waiting for a turn...");
        /* Other players turns */
        if (msgr->wait_for_turn(opt_.player->id, &playing_id))
        {
            if (is_spectator(playing_id))
            {
                DEBUG("Turn for spectator %d, never mind...", playing_id);
                continue;
            }

            DEBUG("Turn for player %d (not me)", playing_id);

            /* Get current player actions */
            Actions* actions = get_actions();
            actions->clear();
            DEBUG("Getting actions...");
            msgr->pull_actions(actions);
            DEBUG("Got %u actions", actions->size());

            /* Apply them onto the gamestate */
            for (auto action : actions->actions())
                apply_action(action);

            /* End of each move */
            end_of_player_turn(playing_id);
        }
        else /* Current player turn */
        {
            DEBUG("Turn for spectator %d (me!!!)", playing_id);
            get_actions()->clear();
            spectator_turn();
            /* The server is waiting for this spectator actions, so the
             * spectator can (on intend) hang some time so the user can inspect
             * the state of the game. */
            Actions* actions = get_actions();
            DEBUG("Sending %u actions...", actions->size());
            msgr->send_actions(*actions);
            DEBUG("Waiting for acknowledgement...");
            msgr->wait_for_ack();
            /* The server do not publish spectators' actions: do not try to
             * pull them.  */
        }
        /* End of each turn */
        if (last_turn)
        {
            /* If that was the last turn, stop there. */
            DEBUG("That was the last turn, bye!");
            break;
        }
        if (last_player_id == playing_id)
        {
            DEBUG("End of turn!");
            end_of_turn();
            if (!is_finished())
                start_of_turn();
            else
            {
                DEBUG("The next turn will be the last one!");
                last_turn = true;
            }
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

    start_of_turn();
    while (!is_finished())
    {
        for (unsigned int i = 0; i < players_->players.size(); i++)
        {
            if (players_->players[i]->nb_timeout < max_consecutive_timeout)
            {
                DEBUG("Turn for player %d", players_->players[i]->id);
                msgr->push_id(players_->players[i]->id);
                Actions* actions = get_actions();
                actions->clear();
                if (!msgr->poll(timeout_))
                {
                    players_->players[i]->nb_timeout++;
                    DEBUG("Timeout reached, never mind: %d", players_->players[i]->nb_timeout);
                }
                else
                {
                    DEBUG("Getting actions...");
                    msgr->recv_actions(actions);
                    DEBUG("Got %u actions", actions->size());
                    DEBUG("Acknowledging...");
                    msgr->ack();

                    for (auto action: actions->actions())
                        apply_action(action);
                }

                DEBUG("Alright, publish actions");
                msgr->push_actions(*actions);
            }

            end_of_player_turn(players_->players[i]->id);

            /* Spectators must be able to see the state of the game between
             * after each player has finished its turn. */
            for (unsigned int i = 0; i < spectators_->players.size(); i++)
            {
                DEBUG("Turn for spectator %d", spectators_->players[i]->id);
                msgr->push_id(spectators_->players[i]->id);
                Actions* actions = get_actions();
                actions->clear();
                DEBUG("Receiving its only Ack action...");
                msgr->recv_actions(actions);
                DEBUG("Acknowledging...");
                msgr->ack();

                end_of_player_turn(spectators_->players[i]->id);
            }
        }

        end_of_turn();
        DEBUG("End of turn!");
        if (!is_finished())
            start_of_turn();
        else
            break; // Avoid calling is_finished() twice
    }

    at_end();
    at_server_end();
}

} // namespace rules
