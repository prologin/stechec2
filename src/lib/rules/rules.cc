#include "rules.hh"

#include <unordered_set>
#include <set>

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

void SynchronousRules::player_loop(ClientMessenger_sptr msgr)
{
    at_start();
    at_player_start();

    while (!is_finished())
    {
        start_of_round();
        Actions* actions = get_actions();
        actions->clear();

        player_turn();
        msgr->send_actions(*actions);
        msgr->wait_for_ack();
        actions->clear();

        msgr->pull_actions(actions);

        /* Apply actions onto the gamestate */
        /* We should already have applied our actions */
        for (auto action : actions->actions())
            if (action->player_id() != opt_.player->id)
                apply_action(action);

        end_of_round();
    }

    at_end();
    at_player_end();
}

void SynchronousRules::spectator_loop(ClientMessenger_sptr msgr)
{
    at_start();
    at_spectator_start();

    bool last_round = false;

    while (last_round || !is_finished())
    {
        start_of_round();
        Actions* actions = get_actions();
        actions->clear();

        spectator_turn();
        msgr->send_actions(*actions);
        msgr->wait_for_ack();
        actions->clear();

        if (last_round)
            break;

        msgr->pull_actions(actions);
        for (auto action : actions->actions())
            apply_action(action);
        actions->clear();

        end_of_round();
    }

    at_end();
    at_spectator_end();
}

void SynchronousRules::server_loop(ServerMessenger_sptr msgr)
{
    at_start();
    at_server_start();

    std::unordered_set<uint32_t> spectators_ids;
    for (auto spectator : spectators_->players)
        spectators_ids.insert(spectator->id);

    std::unordered_set<uint32_t> players_ids;
    for (auto player : players_->players)
        players_ids.insert(player->id);
    for (auto spectator : spectators_->players)
        players_ids.insert(spectator->id);

    std::set<uint32_t> players_timeouting;
    unsigned int players_count = players_->players.size() +
        spectators_->players.size();

    while (!is_finished())
    {
        unsigned int spectators_count = spectators_->players.size();
        start_of_round();

        Actions* actions = get_actions();
        actions->clear();

        players_timeouting.clear();
        for (auto player : players_->players)
            players_timeouting.insert(player->id);

        for (unsigned int i = 0; i < players_count; ++i)
        {
            if (!msgr->poll(timeout_))
                break;

            // Ignore revived players
            do
            {
                msgr->recv_actions(actions);
                msgr->ack();
            } while (players_ids.find(msgr->last_client_id()) == players_ids.end());

            unsigned player_id = msgr->last_client_id();
            if (spectators_ids.find(player_id) != spectators_ids.end())
                spectators_count--;
            else
                players_timeouting.erase(player_id);
        }

        // Increase timeout count for players that did not answer in time
        for (auto player_id : players_timeouting)
        {
            uint32_t player_id_timeouting = 0;
            for (uint32_t i = 0; i < players_->players.size(); ++i)
            {
                if (player_id == players_->players[i]->id)
                {
                    player_id_timeouting = i;
                    break;
                }
            }

            players_->players[player_id_timeouting]->nb_timeout++;

            if (players_->players[player_id_timeouting]->nb_timeout == 3)
            {
                players_ids.erase(player_id);
                players_count--;
            }
        }

        // No timeout for spectators
        while (spectators_count > 0)
        {
            msgr->recv_actions(actions);
            msgr->ack();
            spectators_count--;
        }

        for (auto action: actions->actions())
            apply_action(action);
        msgr->push_actions(*actions);

        end_of_round();
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

void TurnBasedRules::player_loop(ClientMessenger_sptr msgr)
{
    uint32_t last_player_id;
    msgr->pull_id(&last_player_id);

    at_start();
    at_player_start();

    start_of_round();
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
                start_of_spectator_turn(playing_id);
                start_of_turn(playing_id);
                end_of_spectator_turn(playing_id);
                end_of_turn(playing_id);
                continue;
            }

            DEBUG("Turn for player %d (not me)", playing_id);
            start_of_player_turn(playing_id);
            start_of_turn(playing_id);

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
            start_of_player_turn(playing_id);
            start_of_turn(playing_id);

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
        end_of_player_turn(playing_id);
        end_of_turn(playing_id);

        /* End of each round */
        if (last_player_id == playing_id)
        {
            DEBUG("End of turn!");
            end_of_round();
            if (!is_finished())
                start_of_round();
            else
                break; // Avoid calling is_finished() twice
        }
    }

    at_end();
    at_player_end();
}

void TurnBasedRules::spectator_loop(ClientMessenger_sptr msgr)
{
    bool last_round = false;
    uint32_t last_player_id;
    msgr->pull_id(&last_player_id);

    at_start();
    at_spectator_start();

    start_of_round();
    /* `last_round` allows us to inspect the final state of the game, when no
     * other player can play anymore. */
    while (last_round || !is_finished())
    {

        uint32_t playing_id;

        DEBUG("Waiting for a turn...");
        /* Other players turns */
        if (msgr->wait_for_turn(opt_.player->id, &playing_id))
        {
            if (is_spectator(playing_id))
            {
                DEBUG("Turn for spectator %d, never mind...", playing_id);
                start_of_spectator_turn(playing_id);
                start_of_turn(playing_id);
                end_of_spectator_turn(playing_id);
                end_of_turn(playing_id);
                continue;
            }

            DEBUG("Turn for player %d", playing_id);
            start_of_player_turn(playing_id);
            start_of_turn(playing_id);

            /* Get current player actions */
            Actions* actions = get_actions();
            actions->clear();
            DEBUG("Getting actions...");
            msgr->pull_actions(actions);
            DEBUG("Got %u actions", actions->size());

            /* Apply them onto the gamestate */
            for (auto action : actions->actions())
                apply_action(action);

            end_of_player_turn(playing_id);
        }
        else /* Current player turn */
        {
            DEBUG("Turn for spectator %d (me!!!)", playing_id);
            start_of_spectator_turn(playing_id);
            start_of_turn(playing_id);

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
            end_of_spectator_turn(playing_id);
        }

        end_of_turn(playing_id);

        /* End of each round */
        if (last_round)
        {
            /* If that was the last round, stop there. */
            DEBUG("That was the last turn, bye!");
            break;
        }
        if (last_player_id == playing_id)
        {
            DEBUG("End of round!");
            end_of_round();
            if (!is_finished())
                start_of_round();
            else
            {
                DEBUG("The next round will be the last one!");
                last_round = true;
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

    start_of_round();
    while (!is_finished())
    {
        for (auto& p : players_->players)
        {
            start_of_player_turn(p->id);
            start_of_turn(p->id);

                DEBUG("Turn for player %d", p->id);
                msgr->push_id(p->id);
                Actions* actions = get_actions();
                actions->clear();

                if (p->nb_timeout < max_consecutive_timeout)
                {
                    if (!msgr->poll(timeout_))
                    {
                        p->nb_timeout++;
                        DEBUG("Timeout reached, never mind: %d", p->nb_timeout);
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
                }

                msgr->push_actions(*actions);

            end_of_player_turn(p->id);
            end_of_turn(p->id);

            /* Spectators must be able to see the state of the game between
             * after each player has finished its turn. */
            for (auto& s : spectators_->players)
            {
                start_of_spectator_turn(s->id);
                start_of_turn(s->id);

                DEBUG("Turn for spectator %d", s->id);
                msgr->push_id(s->id);
                Actions* actions = get_actions();
                actions->clear();
                DEBUG("Receiving its only Ack action...");
                msgr->recv_actions(actions);
                DEBUG("Acknowledging...");
                msgr->ack();

                end_of_spectator_turn(s->id);
                end_of_turn(s->id);
            }
        }

        end_of_round();
        DEBUG("End of round!");
        if (!is_finished())
            start_of_round();
        else
            break; // Avoid calling is_finished() twice
    }

    at_end();
    at_server_end();
}

} // namespace rules
