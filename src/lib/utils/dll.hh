#ifndef LIB_UTILS_DLL_HH_
#define LIB_UTILS_DLL_HH_

// Dynamic library loader. Used to load rules modules, for example.

#include <stdexcept>
#include <string>

namespace utils {

class DLLError : public std::runtime_error
{
public:
    DLLError();
};

class DLL
{
public:
    explicit DLL(const std::string& filename);
    virtual ~DLL();

    DLL(const DLL&) = delete;
    void operator=(const DLL&) = delete;

    // Gets a symbol from the DLL and automatically casts it to the wanted
    // type. Most likely T will be a function pointer type.
    template <typename T>
    T get(const std::string& sym)
    {
        return reinterpret_cast<T>(get_untyped(sym));
    }

private:
    // Gets an untyped symbol (void* return value).
    void* get_untyped(const std::string& sym);

    std::string filename_;
    void* handle_;
};

} // namespace utils

#endif
