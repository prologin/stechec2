#ifndef LIB_UTILS_SANDBOX_HH_
# define LIB_UTILS_SANDBOX_HH_

// Sandbox used to execute functions in a restricted environment. Currently
// implements timeout only (no memory limit).

# include <functional>
# include <stdexcept>

namespace utils {

class SandboxTimeout : public std::runtime_error
{
public:
    SandboxTimeout() : std::runtime_error("Sandbox call timed out") {}
};

class Sandbox
{
public:
    // Timeout in ms.
    explicit Sandbox(unsigned int timeout = 1000) : timeout_(timeout)
    {
    }

    ~Sandbox() {}

    unsigned int get_timeout() const { return timeout_; }
    void set_timeout(unsigned int timeout) { timeout_ = timeout; }

    // Executes a function in the sandbox. Raises a SandboxTimeout exception on
    // timeout.
    template <typename Ret, typename... Args>
    Ret execute(const std::function<Ret(Args...)>& func, Args... args);

    template <typename Ret, typename... Args>
    Ret execute(Ret (*func)(Args...), Args... args)
    {
        return execute(std::function<Ret(Args...)>(func), args...);
    }

    // Specialized overloads for functions returning void... C++.
    template <typename... Args>
    void execute(const std::function<void(Args...)>& func, Args... args);

private:
    unsigned int timeout_;
};

} // namespace utils

// Implementation of large templated member functions.
# include "sandbox.inl"

#endif // !LIB_UTILS_SANDBOX_HH_
