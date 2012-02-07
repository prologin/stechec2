#ifndef STATE_HH_
# define STATE_HH_

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

private:
    // The older version of this state.
    // Basically, each state is linked to its older version so that an action
    // can be simply cancelled by deleting the current version and reusing the
    // older version.
    RulesState* old_version_;
};

#endif // !STATE_HH_
