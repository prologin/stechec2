// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2019 Association Prologin <association@prologin.org>
#pragma once

#include <memory>

#include "actions.hh"
#include "game-state-history.hh"
#include "player.hh"

namespace rules {

template <typename GameState, typename ApiError>
class Api
{
protected:
    // Functor template implementing the boilerplate of applying a champion's
    // action to the game state. The T template parameter is the first argument
    // of the Action's constructor. It is optional, but declaring it allows
    // using brace-initializers to type T when calling operator().
    template <typename Action, typename T = Action>
    class ApiActionFunc
    {
    public:
        ApiActionFunc(Api* api) : api_(api) {}
        virtual ~ApiActionFunc() = default;

        // Apply the champion's action to the game state
        template <typename... Args>
        ApiError call(Args&&... args)
        {
            // Build action object
            const auto action = std::make_shared<Action>(
                std::forward<Args>(args)..., api_->player_->id);

            // Check action
            ApiError err;
            if ((err = api_->game_state_check(action)) != ApiError::OK)
                return err;

            // Apply action
            api_->actions_.add(action);
            api_->game_state_save();
            api_->game_state_apply(action);
            return ApiError::OK;
        }

        // Universal forward
        template <typename... Args>
        ApiError operator()(Args&&... args)
        {
            return call(std::forward<Args>(args)...);
        }

        // Explicity definining argument type to allow brace-initializing T,
        // which is not otherwise usable with the universal forward defined
        // above.
        ApiError operator()(T arg) { return call(arg); }

    private:
        Api* api_;
    };

public:
    Api(std::unique_ptr<GameState> game_state, Player_sptr player)
        : game_state_(std::move(game_state)), player_(player)
    {}
    virtual ~Api() {}

    const Player_sptr player() const { return player_; }
    void player_set(Player_sptr player) { player_ = player; }

    Actions* actions() { return &actions_; }

    GameState& game_state() { return *game_state_; }
    const GameState& game_state() const { return *game_state_; }

    // Checks action on GameState
    ApiError game_state_check(IAction_sptr action) const
    {
        return static_cast<ApiError>(game_state_->check(action));
    }

    // Applies action to GameState
    void game_state_apply(IAction_sptr action) { game_state_->apply(action); }

    void game_state_save() { game_state_.save(); }
    void clear_old_game_states() { game_state_.clear_old_versions(); }

    // Cancels the last played action
    bool cancel()
    {
        if (!game_state_.can_cancel())
            return false;
        actions_.cancel();
        game_state_.cancel();
        return true;
    }

protected:
    GameStateHistory<GameState> game_state_;
    Player_sptr player_;
    Actions actions_;
};

} // namespace rules
