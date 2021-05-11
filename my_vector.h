#ifndef MY_VECTOR_H
#define MY_VECTOR_H

#include <utility>
#include <iterator>
#include <initializer_list>
#include <cstddef>
#include <stdexcept>

// Interface : https://en.cppreference.com/w/cpp/container/vector

namespace cpp_training {

template <typename T>
class my_vector {
    class my_iterator;
    class my_const_iterator;

public:
    static constexpr double CapacityFactor = 1.5;
    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using const_reference = const T&;
    using const_pointer = const T*;
    using iterator = my_iterator;
    using const_iterator = my_const_iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:

    my_vector() {
    }

    explicit my_vector(size_t size, const T& init_value = T())
        : m_size(size), m_capacity (size * CapacityFactor) {
        auto raw_buff_p = ::operator new (m_capacity * sizeof(T));
        m_buffer_p = static_cast<T*>(raw_buff_p);
        for (int i =0; i<m_size; ++i) {
            new (m_buffer_p + i) T {init_value};
        }
    }

    ~my_vector() noexcept {
        destroy();
    }

    my_vector(const my_vector& rhs) {
        grow_and_copy_from(rhs.capacity(), rhs);
    }

    my_vector(iterator begin, iterator end) {
        reserve((end - begin) * CapacityFactor);
        while (begin != end) {
            push_back(*begin++);
        }
    }

    template <typename InIter>
    my_vector(InIter begin, InIter end) {
        for (; begin != end; ++begin) {
            push_back(*begin);
        }
    }

    my_vector(my_vector&& rhs) : m_size{rhs.m_size}, m_capacity{rhs.m_capacity}, m_buffer_p{rhs.m_buffer_p} {
        new (&rhs) my_vector(0);
    }

    my_vector( std::initializer_list<T> lst )
        : m_size{lst.size()}, m_capacity {static_cast<size_t>(m_size* CapacityFactor)} {
        auto raw_buff_p = ::operator new (m_capacity * sizeof(T));
        m_buffer_p = static_cast<T*>(raw_buff_p);
        auto it = lst.begin();
        for (int i =0; i<m_size; ++i) {
            new (m_buffer_p + i) T { *it++ };
        }
    }

    my_vector& operator = (const my_vector& rhs) {
        my_vector tmp (rhs);
        swap(tmp);
        return *this;
    }

    my_vector& operator = (my_vector&& rhs) {
        if (this == &rhs) return *this;
        destroy();
        m_size = rhs.m_size;
        m_capacity = rhs.m_capacity;
        m_buffer_p = rhs.m_buffer_p;
        new (&rhs) my_vector(0);
        return *this;
    }

    void reserve(size_t new_cap) {
        if (new_cap > m_capacity) {
            grow_and_copy_from(new_cap, *this);
        }
    }

    T& operator [] (int i) {
        return m_buffer_p[i];
    }

    const T& operator [] (int i) const {
        return m_buffer_p[i];
    }

    T& at (size_t pos) {
        if (pos >= m_size) throw std::out_of_range("pos is out of range");
        return m_buffer_p[pos];
    }

    const T& at (size_t pos) const {
        if (pos >= m_size) throw std::out_of_range("pos is out of range");
        return m_buffer_p[pos];
    }

    void push_back (const T& rhs) {
        if (m_size == m_capacity) {
            grow_and_copy_from((m_size + 1) * CapacityFactor, *this);
        }
        new (m_buffer_p + m_size) T {rhs};
        m_size++;
    }

    void push_back (T&& rhs) {
        if (m_size == m_capacity) {
            grow_and_copy_from((m_size + 1) * CapacityFactor, *this);
        }
        new (m_buffer_p + m_size) T{ std::move(rhs) };
        m_size++;
    }

    // Appends a new element to the end of the container.
    // typically uses placement-new to construct the element in-place at the location provided by the container.
    // The arguments args... are forwarded to the constructor as std::forward<Args>(args)...
    template< class... Args >
    void emplace_back( Args&&... args ) {
        if (m_size == m_capacity) {
            grow_and_copy_from((m_size + 1) * CapacityFactor, *this);
        }
        new (m_buffer_p + m_size) T{ std::forward<Args>(args)... };
        m_size++;
    }

    void pop_back () {
        if (!is_empty()) {
            m_buffer_p[m_size-1].~T();
            m_size--;
        }
    }

    size_t size() const {
        return m_size;
    }

    size_t capacity() const {
        return m_capacity;
    }

    bool is_empty() const {
        return m_size == 0;
    }

    void clear() {
        for (int i=0; i<m_size; ++i) {
            m_buffer_p[i].~T();
        }
        m_size = 0;
    }

    void swap(my_vector& rhs) noexcept {
        std::swap(m_size, rhs.m_size);
        std::swap(m_capacity, rhs.m_capacity);
        std::swap(m_buffer_p, rhs.m_buffer_p);
    }

    //    Resizes the container to contain count elements.
    //    If the current size is greater than count, the container is reduced to its first count elements.
    //    If the current size is less than count,
    //    1) additional default-inserted elements are appended
    //    T must meet the requirements of MoveInsertable and DefaultInsertable in order to use overload (1).
    //   -T must meet the requirements of CopyInsertable in order to use overload (2).
    void resize(size_t count, const T& value = T()) {
        if (count < m_size) {
            auto cnt = count;
            while (cnt < m_size) {
                m_buffer_p[cnt++].~T();
            }
        } else if (count > m_size) {
            if (count > m_capacity)
                reserve (count * CapacityFactor);
            for (int i=m_size; i<(count-m_size); ++i)
                m_buffer_p[i] = value;
        }
        m_size = count;
    }

    //Requests the removal of unused capacity.
    // It is a non-binding request to reduce capacity() to size(). It depends on the implementation whether the request is fulfilled.
    // If reallocation occurs, all iterators, including the past the end iterator, and all references to the elements are invalidated. If no reallocation takes place, no iterators or references are invalidated.
    void shrink_to_fit() {
        grow_and_copy_from(m_size, *this);
    }

    //inserts value before pos
    iterator insert(const_iterator pos, const T& value) {
        if (pos == cend()) {
            push_back(value);
            return end()-1;
        }
        auto ipos = pos - cbegin();
        if (m_size == m_capacity)
            grow_and_copy_from((m_size + 1) * CapacityFactor, *this);

        auto curr = end();
        while ((curr - begin()) != ipos) {
            *curr = std::move(*(curr-1));
            --curr;
        }
        *curr = value;
        ++m_size;
        return curr;
    }

    //inserts elements from range [first, last) before pos.
    template< class InputIt >
    iterator insert( const_iterator pos, InputIt first, InputIt last ) {
        auto count = std::distance(first, last);
        if (pos == cend()) {
            if (m_size + count > m_capacity)
                grow_and_copy_from((m_size + count) * CapacityFactor, *this);
            auto rit = end();
            while (first != last) {
                push_back(*first++);
            }
            return rit;
        }

        auto ipos = pos - begin();
        if (m_size + count > m_capacity)
            grow_and_copy_from((m_size + count) * CapacityFactor, *this);
        pos = begin() + ipos;
        auto cur = end() - 1;
        while (cur != pos-1) {
            *(cur + count) = std::move(*cur);
            --cur;
        }
        cur = begin() + ipos;
        while (first != last) {
            *cur++ = *first++;
        }
        m_size += count;
        return begin() + ipos;
    }

    // Removes the element at pos.
    // Return Iterator following the last removed element.
    // If pos refers to the last element, then the end() iterator is returned.
    iterator erase( const_iterator pos ) {
        if (!is_empty() && pos != end()) {
            auto cur = begin() + (pos-cbegin());
            cur->~T();
            while ((cur) != end()) {
                *cur = std::move(*(cur + 1));
                ++cur;
            }
            m_size--;
        }
        return begin() + (pos-cbegin());
    }

    // Removes the elements in the range [first, last).
    iterator erase( const_iterator first, const_iterator last ) {
        auto count = last - first;
        if (!is_empty() && count > 0) {
            auto it = begin() + (first - cbegin());
            auto it2 = begin() + (last - cbegin());
            while (it != last) {
                it->~T();
                *it++ = std::move(*it2++);
            }
            it2 = it + count;
            while (it2 < end()) {
                *it++ = std::move(*it2++);
            }
            m_size -= count;
            return begin() + (first - begin());
        }
        return begin();
    }

    T& front() {
        return m_buffer_p[0];
    }

    T& back() {
        return m_buffer_p[m_size-1];
    }

    const T& front() const {
        return m_buffer_p[0];
    }

    const T& back() const {
        return m_buffer_p[m_size-1];
    }

    iterator begin() noexcept {
        return my_iterator(m_buffer_p);
    }

    const_iterator begin() const noexcept {
        return my_const_iterator(m_buffer_p);
    }

    iterator end() noexcept {
        return my_iterator(m_buffer_p + m_size);
    }

    const_iterator end() const noexcept {
        return my_const_iterator(m_buffer_p + m_size);
    }

    const_iterator cbegin() const noexcept { return my_const_iterator(m_buffer_p); }

    const_iterator cend() const noexcept { return my_const_iterator(m_buffer_p + m_size); }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

    const_reverse_iterator rcbegin() const noexcept { return const_reverse_iterator(cend()); }

    const_reverse_iterator rcend() const noexcept { return const_reverse_iterator(cbegin()); }

    bool operator == (const my_vector<T>& rhs) const {
        if (m_size != rhs.size()) return false;

        for (int i=0; i<m_size; ++i) {
            if (!(m_buffer_p[i] == rhs[i]) ) return false;
        }
        return true;
    }

    bool operator != (const my_vector<T>& rhs) const {
        return !(*this == rhs);
    }

    bool operator < (const my_vector<T>& rhs) const {
        auto min_sz = std::min(m_size, rhs.size());

        for (int i = 0; i<min_sz; ++i) {
            if (m_buffer_p[i] < rhs[i]) return true;
            else if (m_buffer_p[i] > rhs[i]) return false;
        }
        return m_size < rhs.size();
    }

    bool operator <= (const my_vector<T>& rhs) const {
        auto min_sz = std::min(m_size, rhs.size());
        for (int i = 0; i<min_sz; ++i) {
            if (m_buffer_p[i] < rhs[i]) return true;
            else if (m_buffer_p[i] > rhs[i]) return false;
        }
        return m_size <= rhs.size();
    }

    bool operator > (const my_vector<T>& rhs) const {
        return !(*this <= rhs);
    }

    bool operator >= (const my_vector<T>& rhs) const {
        return !(*this < rhs);
    }

private:
    // Destroy this object calling destructors
    void destroy () {
        for (int i=0; i<m_size; ++i) {
            m_buffer_p[i].~T();
        }
        ::operator delete (m_buffer_p);
    }

    void grow_and_copy_from (size_t new_cap, const my_vector& source) {
        auto rawbuff_p = ::operator new (new_cap * sizeof(T));
        auto new_size = source.m_size;
        if (this == &source) {
            // Moving elements when reallocating itself
            for (int i=0; i<source.m_size; ++i) {
                new (static_cast<char*>(rawbuff_p) + i*sizeof(T)) T { std::move(source.m_buffer_p[i]) };
            }
            ::operator delete (m_buffer_p);
        } else {
            for (int i=0; i<source.m_size; ++i) {
                new (static_cast<char*>(rawbuff_p) + i*sizeof(T)) T {source.m_buffer_p[i]};
            }
            // destroy this container by calling destructors
            destroy();
        }
        m_buffer_p = static_cast<T*>(rawbuff_p);
        m_capacity = new_cap;
        m_size = new_size;
    }

    //
    // Iterator, see https://en.cppreference.com/w/cpp/iterator/iterator
    //
    class my_iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using reference = T&;
        using pointer = T*;
    public:
        explicit my_iterator (T * ptr) : cur_p(ptr) {}
        my_iterator operator ++ (int) { return my_iterator(cur_p++); }
        my_iterator& operator ++ () { cur_p++; return *this; }
        my_iterator operator -- (int) { return my_iterator(cur_p--); }
        my_iterator& operator -- () { cur_p--; return *this; }
        difference_type operator - (my_iterator rhs) const { return cur_p - rhs.cur_p; }
        my_iterator& operator += (difference_type n) { cur_p += n; return *this; }
        my_iterator& operator -= (difference_type n) { cur_p -= n; return *this; }
        my_iterator operator - (difference_type n) const { return my_iterator(cur_p - n); }
        my_iterator operator + (difference_type n) const { return my_iterator(cur_p + n); }
        pointer operator -> () { return cur_p; }
        reference operator * () { return *cur_p; }
        reference operator [] (difference_type n) { return cur_p[n]; }
        bool operator != (my_iterator rhs) const { return cur_p != rhs.cur_p; }
        bool operator == (my_iterator rhs) const { return cur_p == rhs.cur_p; }
        bool operator != (my_const_iterator rhs) const { return cur_p != rhs.cur_p; }
        bool operator < (const my_iterator& rhs) const { return cur_p < rhs.cur_p; }
        bool operator > (const my_iterator& rhs) const { return cur_p > rhs.cur_p; }
        operator my_const_iterator () { return my_const_iterator(cur_p); }
    private:
        T * cur_p = nullptr;
    };

    class my_const_iterator {
        friend class my_iterator;
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = const T;
        using difference_type = ptrdiff_t;
        using reference = const T&;
        using pointer = const T*;
    public:
        explicit my_const_iterator (pointer ptr) : cur_p(ptr) {}
        my_const_iterator operator ++ (int) { return my_const_iterator(cur_p++); }
        my_const_iterator& operator ++ () { cur_p++; return *this; }
        my_const_iterator operator -- (int) { return my_const_iterator(cur_p--); }
        my_const_iterator& operator -- () { cur_p--; return *this; }
        difference_type operator - (my_const_iterator rhs) const { return cur_p - rhs.cur_p; }
        my_const_iterator operator - (difference_type n) const { return my_const_iterator(cur_p - n); }
        my_const_iterator operator + (difference_type n) const { return my_const_iterator(cur_p + n); }
        pointer operator -> () { return cur_p; }
        reference operator * () { return *cur_p; }
        bool operator != (my_const_iterator rhs) { return cur_p != rhs.cur_p; }
        bool operator == (my_const_iterator rhs) { return cur_p == rhs.cur_p; }
        bool operator < (const my_const_iterator& rhs) const { return cur_p < rhs.cur_p; }
        bool operator > (const my_const_iterator& rhs) const { return cur_p > rhs.cur_p; }
    private:
        const T * cur_p = nullptr;
    };

private:
    size_t m_size = 0;
    size_t m_capacity = 0;
    T * m_buffer_p = nullptr;
};

}


#endif // MY_VECTOR_H
