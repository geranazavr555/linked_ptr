#ifndef LINKED_PTR_H
#define LINKED_PTR_H

namespace smart_ptr
{
    namespace details
    {
        class intrusive_mixin
        {
        public:
            intrusive_mixin *l = nullptr, *r = nullptr;

        public:
            void attach(intrusive_mixin* copy)
            {
                copy->l = this;
                copy->r = r;
                if (r)
                    r->l = copy;
                r = copy;
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

            void swap(intrusive_mixin &other)
            {
                intrusive_mixin* attach_target_a = (l ? l : r);
                intrusive_mixin* attach_target_b = (other.l ? other.l : other.r);
                detach();
                other.detach();
                if (attach_target_a)
                    attach_target_a->attach(&other);
                if (attach_target_b)
                    attach_target_b->attach(this);
            }
        };
    }

    using namespace details;

    template <typename T>
    class linked_ptr
    {
        template <typename U>
        friend class linked_ptr;

    private:
        mutable intrusive_mixin lnk;
        T* pointer;

    public:
// constructors / destructor
        constexpr linked_ptr() noexcept : lnk{nullptr, nullptr}, pointer(nullptr) {}

        linked_ptr(T* pointer) noexcept : lnk{nullptr, nullptr}, pointer(pointer) {}

        linked_ptr(linked_ptr const& other) : lnk{nullptr, nullptr}, pointer(other.get())
        {
            other.attach(*this);
            //attach(other);
        }
/*
        template <typename U, typename = std::enable_if<std::is_convertible_v<U*, T*>>>
        linked_ptr(linked_ptr<U> const& other) : l(nullptr), r(nullptr), pointer(other.get())
        {
            other.attach(*this);
        }*/

        template <typename U, typename = std::enable_if<std::is_convertible_v<U*, T*>>>
        linked_ptr(U* pointer) : lnk{nullptr, nullptr}, pointer(pointer) {}

        template <typename U, typename = std::enable_if<std::is_convertible_v<U*, T*>>>
        linked_ptr(linked_ptr<U> const& other) : lnk{nullptr, nullptr}, pointer(other.get())
        {
            //other.attach(*this);
           // if constexpr (std::is_base_of<T, U>::value)
            //{
            //    attach(other);
            //}
            //else
            //{
                other.attach(*this);
            //}
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
        linked_ptr& operator=(linked_ptr<U> const& other)
        {
            auto tmp(other);
            swap(tmp);
            return *this;
        }

// common smart pointer interface
        template <typename U = T, typename = std::enable_if<std::is_convertible_v<U*, T*>>>
        void reset(U* new_pointer = nullptr)
        {
            destroy();
            pointer = new_pointer;
        }

        void swap(linked_ptr& other) noexcept
        {
            lnk.swap(other.lnk);
            std::swap(pointer, other.pointer);
        }

        T* get() const noexcept
        {
            return pointer;
        }

        bool unique() const noexcept
        {
            return !lnk.l & !lnk.r && pointer;
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
        template <typename U = T, typename = std::enable_if<std::is_convertible_v<U*, T*>>>
        void attach(linked_ptr<U> const& copy) const noexcept
        {
            lnk.attach(&copy.lnk);
        }

        void detach() const noexcept
        {
            lnk.detach();
        }

        void destroy()
        {
            //enum {T_have_to_be_complete = sizeof(T)};
            if (unique() && pointer)
                delete pointer;
            lnk.detach();
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
