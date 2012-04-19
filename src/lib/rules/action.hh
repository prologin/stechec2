#ifndef LIB_RULES_ACTION_HH_
# define LIB_RULES_ACTION_HH_

# include <memory>
# include <list>
# include <utils/buffer.hh>

# include "game-state.hh"
# include "player.hh"

namespace rules {

// Interface to be implemented by all action types.
class IAction
{
public:
    // Check if the action can be applied to the state. Returns 0 if there is
    // no error, or a rule dependent error value if the action can't be
    // applied.
    virtual int check(const GameState* st) const = 0;

    // Copies the current state and returns a new state with the action applied.
    GameState* apply(GameState* st) const;

    // Handles serialization and deserialization of the Action object to a
    // buffer.
    virtual void handle_buffer(utils::Buffer& buf) = 0;

private:
    // Only applies the action to a given state, without handling action
    // cancellation.
    virtual void apply_on(GameState* st) const = 0;
};

// A template that can be used to implement actions in an easier way without
// dynamic_casts everywhere.
// It's kind of ugly, but it allows a lot more genericity in the codebase.
template <typename TState>
class Action : public IAction
{
public:
    virtual int check(const TState* st) const = 0;
    virtual int check(const GameState* st) const
    {
        return check(dynamic_cast<const TState*>(st));
    }

    TState* apply(GameState* st) const
    {
        return dynamic_cast<TState*>(IAction::apply(st));
    }

protected:
    virtual void apply_on(TState* st) const = 0;
    virtual void apply_on(GameState* st) const
    {
        apply_on(dynamic_cast<TState*>(st));
    }
};

typedef std::shared_ptr<IAction> IAction_sptr;
typedef std::list<IAction_sptr> IActionList;

struct PlayerActions
{
    Player_sptr player;
    IActionList actions;
};

} // namespace rules

#endif // !LIB_RULES_ACTION_HH_
