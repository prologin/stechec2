#ifndef ACTION_HH_
# define ACTION_HH_

// In rules/state.hh
class RulesState;

// In tools/serialize.hh
class Serializer;

// Interface to be implemented by all action types.
class Action
{
public:
    // Check if the action can be applied to the state. Returns 0 if there is
    // no error, or a rule dependent error value if the action can't be
    // applied.
    virtual int check(const RulesState* st) const = 0;

    // Copies the current state and returns a new state with the action applied.
    RulesState* apply(RulesState* st) const;

    // Cancels the action and returns a new state with the action cancelled.
    RulesState* cancel(RulesState* st) const;

    // Serializes the action object by saving all internal parameters to a
    // stream.
    virtual void serialize(Serializer& serializer) const = 0;

private:
    // Only applies the action to a given state, without handling action
    // cancellation.
    virtual void apply_on(RulesState* st) const = 0;
};

#endif // !ACTION_HH_
