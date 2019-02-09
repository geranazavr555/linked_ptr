#ifndef LINKED_PTR_H
#define LINKED_PTR_H

namespace smart_ptr
{
    template <typename T>
    class linked_ptr
    {
    private:
        mutable linked_ptr<T> *l;
        mutable linked_ptr<T> *r;
        T* pointer;

    public:
// constructors / destructor
        constexpr linked_ptr() noexcept : l(nullptr), r(nullptr), pointer(nullptr) {}

        linked_ptr(T* pointer) noexcept : l(nullptr), r(nullptr), pointer(pointer) {}

        linked_ptr(linked_ptr const& other) : l(nullptr), r(nullptr), pointer(other.get())
        {
            other.attach(*this);
        }

        template <typename U, typename = std::enable_if<std::is_convertible_v<U*, T*>>>
        linked_ptr(U* pointer) : l(nullptr), r(nullptr), pointer(pointer) {}

        template <typename U, typename = std::enable_if<std::is_convertible_v<U*, T*>>>
        linked_ptr(linked_ptr<U> const& other) : l(nullptr), r(nullptr), pointer(other.get())
        {
            other.attach(*this);
        }

        ~linked_ptr()
        {
            destroy();
        }

// assign operators
        linked_ptr& operator=(linked_ptr const& other) noexcept
        {
            auto tmp(other);
            swap(tmp);
            return *this;
        }

        template <typename U, typename = std::enable_if<std::is_convertible_v<U*, T*>>>
        linked_ptr& operator=(linked_ptr<U> const& other);

// common smart pointer interface
        template <typename U = T, typename = std::enable_if<std::is_convertible_v<U*, T*>>>
        void reset(U* new_pointer = nullptr)
        {
            destroy();
            pointer = new_pointer;
        }

        void swap(linked_ptr& other) noexcept
        {
            linked_ptr* attach_target_a = (l ? l : r);
            linked_ptr* attach_target_b = (other.l ? other.l : other.r);
            detach();
            other.detach();
            std::swap(pointer, other.pointer);
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
            return !l && !r && pointer;
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
        void attach(linked_ptr const& copy) const noexcept
        {
            copy.l = const_cast<linked_ptr<T>*>(this);
            copy.r = r;
            if (r)
                r->l = const_cast<linked_ptr<T>*>(&copy);
            r = const_cast<linked_ptr<T>*>(&copy);
        }

        void detach() const noexcept
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
            enum {T_have_to_be_complete = sizeof(T)};
            if (unique() && pointer)
                delete pointer;
            detach();
        }
    };


    template <typename T, typename U>
    inline bool operator==(linked_ptr<T> const& a, linked_ptr<U> const& b) noexcept
    {
        return a.get() == b.get();
    }

    template <typename T, typename U>
    inline bool operator!=(linked_ptr<T> const& a, linked_ptr<U> const& b) noexcept
    {
        return !(a == b);
    }

    template <typename T, typename U>
    inline bool operator<(linked_ptr<T> const& a, linked_ptr<U> const& b) noexcept
    {
        return a.get() < b.get();
    }

    template <typename T, typename U>
    inline bool operator>(linked_ptr<T> const& a, linked_ptr<U> const& b) noexcept
    {
        return b < a;
    }

    template <typename T, typename U>
    inline bool operator<=(linked_ptr<T> const& a, linked_ptr<U> const& b) noexcept
    {
        return a < b || a == b;
    }

    template <typename T, typename U>
    inline bool operator>=(linked_ptr<T> const& a, linked_ptr<U> const& b) noexcept
    {
        return (b <= a);
    }

    template <typename T>
    void swap(linked_ptr<T> &a, linked_ptr<T> &b) noexcept
    {
        a.swap(b);
    }
}

#endif
