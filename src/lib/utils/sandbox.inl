#include "log.hh"
#include "sandbox.hh"

#include <ctime>
#include <pthread.h>
#include <string>
#include <tuple>

// Implementation of the sandbox. Lots of tricky C++ to get a nice user API,
// but most of it could be removed when C++11 compilers become better.

namespace utils {

// Private namespace for the sandbox implementation.
namespace sandbox_impl {

// Metaprogramming trick to apply a std::tuple to a function.
template <int N>
struct apply_func
{
    template <typename Ret, typename... ArgsF, typename... Args>
    static Ret apply(const std::function<Ret(ArgsF...)>& f,
                     const std::tuple<ArgsF...>& t,
                     Args... a)
    {
        return apply_func<N - 1>::apply(f, t, std::get<N - 1>(t), a...);
    }
};
template <>
struct apply_func<0>
{
    template <typename Ret, typename... ArgsF, typename... Args>
    static Ret apply(const std::function<Ret(ArgsF...)>& f,
                     const std::tuple<ArgsF...>&,
                     Args... a)
    {
        return f(a...);
    }
};
template <typename Ret, typename... Args>
Ret apply_tuple(const std::function<Ret(Args...)>& f,
                const std::tuple<Args...>& t)
{
    return apply_func<sizeof...(Args)>::apply(f, t);
}

// Wraps a function which returns void as a function which returns an useless
// int. C++ sucks.
template <int N>
struct apply_void_func
{
    template <typename... ArgsF, typename... Args>
    static void apply(const std::function<void(ArgsF...)>& f,
                      const std::tuple<ArgsF...>& t,
                      Args... a)
    {
        apply_void_func<N - 1>::apply(f, t, std::get<N - 1>(t), a...);
    }
};
template <>
struct apply_void_func<0>
{
    template <typename... ArgsF, typename... Args>
    static void apply(const std::function<void(ArgsF...)>& f,
                      const std::tuple<ArgsF...>&,
                      Args... a)
    {
        f(a...);
    }
};
template <typename... Args>
std::function<int(Args...)> wrap_void(const std::function<void(Args...)>& f)
{
    return [f] (Args... a) {
        apply_void_func<sizeof...(Args)>::apply(f, std::make_tuple(a...));
        return 0;
    };
}

// Thread procedure in which the sandboxed code is executed.
template <typename Ret, typename... Args>
struct ThreadParams
{
    Sandbox* sandbox;
    std::function<Ret(Args...)> func;
    std::tuple<Args...> args;
    Ret* retptr;
};
template <typename Ret, typename... Args>
void* thread_proc(void* arg)
{
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    ThreadParams<Ret, Args...>* p = (ThreadParams<Ret, Args...>*)arg;
    *p->retptr = apply_tuple(p->func, p->args);
    return nullptr;
}

} // namespace sandbox_impl

template <typename Ret, typename... Args>
Ret Sandbox::execute(const std::function<Ret(Args...)>& func, Args... args)
{
    if (get_timeout() == 0)
        return func(args...);

    // Initialize the thread parameters object.
    std::tuple<Args...> args_t = std::make_tuple(args...);
    Ret retval;
    sandbox_impl::ThreadParams<Ret, Args...> params {
        this, func, args_t, &retval
    };

    // Initialize the timeout struct.
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) < 0)
        FATAL("Unable to get the current time");
    ts.tv_sec += get_timeout() / 1000;
    ts.tv_nsec += (get_timeout() % 1000) * 1000000;
    if (ts.tv_nsec >= 1000000000L)
    {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000L;
    }

    // Create the thread.
    pthread_t sandbox_thread;
    if (pthread_create(&sandbox_thread, NULL,
                       sandbox_impl::thread_proc<Ret, Args...>, &params) < 0)
        FATAL("Sandbox thread creation failed");

    // Wait until timeout or normal return.
    if (pthread_timedjoin_np(sandbox_thread, NULL, &ts) == ETIMEDOUT)
    {
        ERR("Sandbox call exceeded the time limit");
        pthread_cancel(sandbox_thread);
        throw SandboxTimeout();
    }

    return retval;
}

template <typename... Args>
void Sandbox::execute(const std::function<void(Args...)>& func, Args... args)
{
    execute(sandbox_impl::wrap_void(func), args...);
}

} // namespace utils
