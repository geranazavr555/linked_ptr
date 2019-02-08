#ifndef LINKED_PTR_H
#define LINKED_PTR_H

template <typename T>
class linked_ptr
{
private:
    mutable linked_ptr<T> *l, *r;
    T* pointer;

public:
// constructors / destructor
    linked_ptr() : l(nullptr), r(nullptr), pointer(nullptr) {}

    linked_ptr(T* pointer) : l(nullptr), r(nullptr), pointer(pointer) {}

    linked_ptr(linked_ptr const& other) : l(nullptr), r(nullptr), pointer(other.get())
    {
        other.attach(&this);
    }

    template <typename U>
    linked_ptr(U* pointer) : l(nullptr), r(nullptr), pointer(pointer) {}

    template <typename U>
    linked_ptr(linked_ptr<U> const& other) : l(nullptr), r(nullptr), pointer(other.get())
    {
        other.attach(*this);
    }

    ~linked_ptr()
    {
        destroy();
    }

// assign operators
    linked_ptr& operator=(linked_ptr const& other)
    {
        auto tmp(other);
        swap(tmp);
        return *this;
    }

    template <typename U>
    linked_ptr& operator=(linked_ptr<U> const& other);

// common smart pointer interface
    template <typename U>
    void reset(U* new_pointer = nullptr)
    {
        destroy();
        pointer = new_pointer;
    }

    void swap(linked_ptr& other)
    {
        linked_ptr* attach_target_a = (l ? l : r);
        linked_ptr* attach_target_b = (other.l ? other.l : other.r);
        detach();
        other.detach();
        swap(pointer, other.pointer);
        if (attach_target_a)
            attach_target_a->attach(other);
        if (attach_target_b)
            attach_target_b->attach(*this);
    }

    T* get() const noexcept
    {
        return pointer;
    }

    bool unique() const noexcept
    {
        return !l && !r;
    }

    operator bool() const noexcept
    {
        return get();
    }

// pointer using interface
    T& operator*() const
    {
        return *get();
    }

    T* operator->() const
    {
        return get();
    }

private:
    void attach(linked_ptr<T>& copy)
    {
        copy.l = this;
        copy.r = r;
        r->l = &copy;
        r = &copy;
    }

    void detach()
    {
        if (l)
            l->r = r;
        if (r)
            r->l = l;
        l = nullptr;
        r = nullptr;
    }

    void destroy()
    {
        if (unique())
            delete pointer;
        detach();
    }
};


template <typename T, typename U>
inline bool operator==(linked_ptr<T> const& a, linked_ptr<U> const& b)
{
    return a.get() == b.get();
}

template <typename T, typename U>
inline bool operator!=(linked_ptr<T> const& a, linked_ptr<U> const& b)
{
    return !(a == b);
}

template <typename T, typename U>
inline bool operator<(linked_ptr<T> const& a, linked_ptr<U> const& b)
{
    return a.get() < b.get();
}

template <typename T, typename U>
inline bool operator>(linked_ptr<T> const& a, linked_ptr<U> const& b)
{
    return b < a;
}

template <typename T, typename U>
inline bool operator<=(linked_ptr<T> const& a, linked_ptr<U> const& b)
{
    return a < b || a == b;
}

template <typename T, typename U>
inline bool operator>=(linked_ptr<T> const& a, linked_ptr<U> const& b)
{
    return (b <= a);
}

template <typename T>
void swap(linked_ptr<T> &a, linked_ptr<T> &b)
{
    a.swap(b);
}

#endif