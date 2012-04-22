#ifndef LIB_UTILS_BUFFER_HH_
# define LIB_UTILS_BUFFER_HH_

# include "log.hh"

# include <cassert>
# include <cstdint>
# include <cstdlib>
# include <cstring>
# include <stdexcept>
# include <string>
# include <vector>

namespace utils {

// Exception raised when a deserialization fails. For example, when the buffer
// is too small to be deserialized properly.
struct DeserializationError : public std::runtime_error
{
    DeserializationError() : std::runtime_error("Deserialization error") {}
};

// Serializes or deserializes data to an internal buffer. To use it, use the
// "handle" method on a variable, or "handle_array" on an array.
class Buffer
{
public:
    // If no buffer is provided, it's serialization
    Buffer() : serialize_(true) {}

    // If a buffer is provided, it's unserialization
    Buffer(const std::vector<uint8_t>& data)
        : data_(data), idx_(0), serialize_(false) {}

    // Allow access to the serialized data
    const uint8_t* data() const { return &data_[0]; }
    size_t size() const { return data_.size(); }
    bool serialize() const { return serialize_; }

    // Handle a memory zone argument
    void handle_mem(char* mem, size_t len)
    {
        if (serialize_)
            data_.insert(data_.end(), mem, mem + len);
        else
        {
            CHECK_EXC(DeserializationError, idx_ + len <= data_.size());
            memcpy(mem, &data_[idx_], len);
            idx_ += len;
        }
    }

    // Handle a string
    void handle(std::string& s)
    {
        unsigned int size = s.size();
        handle(size);

        if (serialize_)
            handle_mem((char*)s.c_str(), size);
        else
        {
            CHECK_EXC(DeserializationError, idx_ + size <= data_.size());
            s.assign(&data_[idx_], &data_[idx_ + size]);
            idx_ += size;
        }
    }

    // Handle arrays
    template <typename T>
    void handle_array(T* arr, size_t count)
    {
        handle_mem((char*)arr, count * sizeof (T));
    }

    // Handle integral and simple types
    template <typename T>
    void handle(T& x)
    {
        handle_mem((char*)&x, sizeof (T));
    }

private:
    std::vector<uint8_t> data_;
    size_t idx_;
    bool serialize_;
};

}

#endif // !LIB_UTILS_BUFFER_HH_
