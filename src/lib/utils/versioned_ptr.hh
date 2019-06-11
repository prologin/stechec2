#pragma once

#include <forward_list>
#include <memory>

namespace utils {

template <typename T>
class VersionedPtr
{
public:
    VersionedPtr(std::unique_ptr<T> current) : current_(std::move(current)) {}

    T& operator*() { return *current_.get(); }
    const T& operator*() const { return *current_.get(); }
    T* operator->() { return current_.get(); }
    const T* operator->() const { return current_.get(); }

    void save() { versions_.emplace_front(new T(*current_)); }
    bool can_cancel() const { return versions_.cbegin() != versions_.cend(); }
    void cancel()
    {
        current_.swap(versions_.front());
        versions_.pop_front();
    }
    void clear_old_versions() { versions_.clear(); }

private:
    std::unique_ptr<T> current_;
    std::forward_list<std::unique_ptr<T>> versions_;
};

} // namespace utils
