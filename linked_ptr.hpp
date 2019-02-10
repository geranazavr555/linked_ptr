#ifndef LINKED_PTR_H
#define LINKED_PTR_H

namespace smart_ptr
{
    namespace details
    {
        class intrusive_mixin
        {
        public:
            volatile intrusive_mixin *l = nullptr;
            volatile intrusive_mixin *r = nullptr;

            intrusive_mixin(intrusive_mixin *l, intrusive_mixin *r) : l(l), r(r) {}
            intrusive_mixin() : intrusive_mixin(nullptr, nullptr) {}

        public:
            void attach(volatile intrusive_mixin* copy) volatile
            {
                copy->l = this;
                copy->r = r;
                if (r)
                    r->l = copy;
                r = copy;
            }

            void detach() volatile
            {
                if (l)
                    l->r = r;
                if (r)
                    r->l = l;
                l = nullptr;
                r = nullptr;
            }

            void swap(volatile intrusive_mixin &other) volatile
            {
                volatile intrusive_mixin* attach_target_a = (l ? l : r);
                volatile intrusive_mixin* attach_target_b = (other.l ? other.l : other.r);
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
        mutable volatile intrusive_mixin intrusive_node;
        T* pointer;

    public:
// constructors / destructor
        constexpr linked_ptr() noexcept : intrusive_node(), pointer(nullptr) {}

        explicit linked_ptr(T* pointer) noexcept : intrusive_node(), pointer(pointer) {}

        linked_ptr(linked_ptr const& other) : intrusive_node(), pointer(other.get())
        {
            other.attach(*this);
        }

        template <typename U, typename = std::enable_if<std::is_convertible_v<U*, T*>>>
        explicit linked_ptr(U* pointer) : intrusive_node(), pointer(pointer) {}

        template <typename U, typename = std::enable_if<std::is_convertible_v<U*, T*>>>
        linked_ptr(linked_ptr<U> const& other) : intrusive_node(), pointer(other.get())
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
            intrusive_node.swap(other.intrusive_node);
            std::swap(pointer, other.pointer);
        }

        T* get() const noexcept
        {
            return pointer;
        }

        bool unique() const noexcept
        {
            return !intrusive_node.l && !intrusive_node.r && pointer;
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
            intrusive_node.attach(&copy.intrusive_node);
        }

        void detach() const noexcept
        {
            intrusive_node.detach();
        }

        void destroy()
        {
            //enum {T_have_to_be_complete = sizeof(T)};
            if (unique() && pointer)
            {
                delete pointer;
            }
            pointer = nullptr;
            intrusive_node.detach();
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
