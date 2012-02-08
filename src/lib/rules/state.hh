#ifndef STATE_HH_
# define STATE_HH_

#include <cassert>

namespace rules {

// Interface of the rules state.
class RulesState
{
public:
    virtual ~RulesState();

    // Copies this state and returns a new state object with the same data.
    virtual RulesState* copy() const = 0;

    // Getter/Setter for the old version of the state.
    // See below for an explanation of the state versions.
    RulesState* get_old_version() const { return old_version_; }
    void set_old_version(RulesState* st) { old_version_ = st; }

    // Checks if the state has something to cancel.
    bool can_cancel() const { return old_version_ != 0; }

private:
    // The older version of this state.
    // Basically, each state is linked to its older version so that an action
    // can be simply cancelled by deleting the current version and reusing the
    // older version.
    RulesState* old_version_;
};

// Cancels the last action that occurred on the state. External to the state
// so it can have a generic return type and delete the old version.
template <typename T>
T* cancel(T* current_version)
{
    assert(current_version->can_cancel());

    T* old_version = dynamic_cast<T*>(current_version->get_old_version());

    // Remove the link so that deleting current_version does not delete the
    // older version.
    current_version->set_old_version(0);
    delete current_version;

    return old_version;
}

} // namespace rules

#endif // !STATE_HH_
