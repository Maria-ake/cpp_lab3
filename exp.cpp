#include <iterator>
#include <cstddef>
#include <compare>

template <typename T>
class contiguous_iterator
 {
private:
    T* ptr_ = nullptr;

public:
    using iterator_category = std::contiguous_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = T*;
    using reference         = T&;

    contiguous_iterator() = default;
    explicit contiguous_iterator(T* ptr) : ptr_(ptr) {}


    reference operator*() const { return *ptr_; }
    pointer operator->() const { return ptr_; }


    contiguous_iterator& operator++() { ptr_; return *this; }
    contiguous_iterator operator(int) { auto tmp = *this; ++ptr_; return tmp; }
    contiguous_iterator& operator--() { --ptr_; return *this; }
    contiguous_iterator operator--(int) { auto tmp = *this; --ptr_; return tmp; }


    contiguous_iterator& operator+=(difference_type n) { ptr_ += n; return *this; }
    contiguous_iterator& operator-=(difference_type n) { ptr_ -= n; return *this; }

    friend contiguous_iterator operator+(contiguous_iterator it, difference_type n) { it += n; return it; }
    friend contiguous_iterator operator+(difference_type n, contiguous_iterator it) { it += n; return it; }
    friend contiguous_iterator operator-(contiguous_iterator it, difference_type n) { it -= n; return it; }

    friend difference_type operator-(const contiguous_iterator& lhs, const contiguous_iterator& rhs) {
        return lhs.ptr_ - rhs.ptr_;
    }


    reference operator[](difference_type n) const { return *(ptr_ + n); }
    auto operator<=>(const contiguous_iterator& other) const = default;
};