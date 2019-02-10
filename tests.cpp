#include "gtest.h"
#include "linked_ptr.hpp"
#include <memory>
#include <set>

using namespace smart_ptr;

TEST(constructors, default_)
{
    linked_ptr<int> i;
    linked_ptr<double> d;

    linked_ptr< linked_ptr<bool> > llb;
}

class ForwardDeclaratedClass;

TEST(constructors, forward_decl_def)
{
    // Must compile
    linked_ptr<ForwardDeclaratedClass> x;
    //extern ForwardDeclaratedClass exfwd;
    //std::shared_ptr<ForwardDeclaratedClass> x(&exfwd);
    std::shared_ptr<ForwardDeclaratedClass> y;
    //std::shared_ptr<int> z(new int(5));
    //std::cout << "kek";
    SUCCEED();
}

TEST(constructors, forward_decl_decl)
{
    // Must compile
    extern linked_ptr<ForwardDeclaratedClass> extern_incomplete;
}

TEST(constructors, pointer_t)
{
    linked_ptr<int> x(new int(5));
}

struct Base
{
    int x;
    Base(int x) : x(x) {}
    virtual int result()
    {
        return x;
    }
};

struct Derived : public Base
{
    int y;
    Derived(int x, int y): Base(x), y(y) {}
    int result() override
    {
        return y;
    }
};

TEST(constructors, pointer_u)
{
    linked_ptr<Base> bp(new Derived(5, 6));
    ASSERT_EQ(bp->result(), 6);
}

TEST(coping, constructor)
{
    linked_ptr<int> x(new int(5));
    linked_ptr<int> y(x);
    ASSERT_EQ(x, y);
}

struct Base1 {};
struct Derived1 : Base1 {};

TEST(copying, constructor2)
{
    linked_ptr<Base1> x = linked_ptr<Derived1>(new Derived1());
    linked_ptr<Base1> y = linked_ptr<Base1>(new Derived1());
    //linked_ptr<Derived1> z = linked_ptr<Base1>(new Derived1());
    //std::shared_ptr<Derived1> z(std::shared_ptr<Base1>(new Derived1()));
}

TEST(coping, assign)
{
    linked_ptr<int> x(new int(5));
    linked_ptr<int> y(new int(6));
    ASSERT_NE(x, y);
    y = x;
    ASSERT_EQ(x, y);
}

struct DestructionDetector
{
    int* cnt;

    DestructionDetector(int* counter) : cnt(counter) {}

    ~DestructionDetector()
    {
        (*cnt)++;
    }
};

TEST(copying, destruction_correctness)
{
    int count = 0;
    {
        linked_ptr<DestructionDetector> x(new DestructionDetector(&count));
        ASSERT_EQ(count, 0);
        auto y(x);
        ASSERT_EQ(count, 0);
    }
    ASSERT_EQ(count, 1);
}

TEST(copying, destruction_after_assign_correctness)
{
    int count = 0;
    {
        linked_ptr<DestructionDetector> x(new DestructionDetector(&count));
        ASSERT_EQ(count, 0);
        linked_ptr<DestructionDetector> y;
        ASSERT_FALSE(y);
        y = x;
        ASSERT_EQ(count, 0);
    }
    ASSERT_EQ(count, 1);
}

TEST(common_interface, bool_operator)
{
    linked_ptr<int> x(new int(5));
    linked_ptr<int> y;
    ASSERT_TRUE(x);
    ASSERT_FALSE(y);
}

TEST(common_interface, reset_nullptr_t)
{
    linked_ptr<int> x(new int(5));
    x.reset();
    ASSERT_FALSE(x);
}

TEST(common_interface, swap)
{
    linked_ptr<int> x(new int(5)), y;
    ASSERT_TRUE(x);
    ASSERT_FALSE(y);
    swap(x, y);
    ASSERT_FALSE(x);
    ASSERT_TRUE(y);
}

TEST(common_interface, get)
{
    int* x = new int(5);
    linked_ptr<int> y(x);
    ASSERT_TRUE(y);
    ASSERT_EQ(x, y.get());
}

TEST(common_interface, simple_unique)
{
    linked_ptr<int> x(new int(5)), y;
    ASSERT_TRUE(x.unique());
    ASSERT_FALSE(y.unique());
    std::shared_ptr<int> xx(new int(5)), yy;
    ASSERT_TRUE(xx.unique());
    ASSERT_FALSE(yy.unique());
}

TEST(common_interface, unique)
{
    linked_ptr<int> x(new int(5));

    ASSERT_TRUE(x.unique());
    {
        linked_ptr<int> y(x);
        {
            linked_ptr<int> z(x);
            ASSERT_FALSE(y.unique());
            ASSERT_FALSE(x.unique());
            ASSERT_FALSE(z.unique());
        }
        ASSERT_FALSE(y.unique());
        ASSERT_FALSE(x.unique());
    }
    ASSERT_TRUE(x.unique());
}

TEST(pointer_using_interface, arrow)
{
    linked_ptr<std::pair<int, int> > x(new std::pair<int, int>(2, 4));
    ASSERT_EQ(x->first, 2);
    ASSERT_EQ(x->second, 4);
}

TEST(pointer_using_interface, star)
{
    int * x = new int(5);
    linked_ptr<int> y(x);
    ASSERT_EQ(*y, 5);
}

TEST(comparison, eq)
{
    int * x = new int(5);
    linked_ptr<int> y(x);
    linked_ptr<int> z(y);
    ASSERT_TRUE(y == z);
    ASSERT_FALSE(y != z);

    y.reset(new int(6));
    ASSERT_FALSE(y == z);
    ASSERT_TRUE(y != z);
}

TEST(comparison, set)
{
    std::set<linked_ptr<int> > q;
    q.insert(linked_ptr<int>(new int(5)));
    q.insert(linked_ptr<int>(new int(5)));
    q.insert(linked_ptr<int>(new int(6)));
    q.insert(linked_ptr<int>(*q.begin()));
    ASSERT_EQ(q.size(), 3);
}

struct A {
    A() {
        a = 0;
    }
    int a;
};

struct B : A {
    B() {
        a = 1;
    }
};

TEST(third_party_tests, base)
{
    bool check = true;

    linked_ptr<int> ptr1(new int(4));
    linked_ptr<int> ptr2(new int(4));

    linked_ptr<int> ptr3(ptr1);

    check *= ptr2.unique();
    check *= !ptr1.unique();

    check *= (ptr1 == ptr3);

    ASSERT_TRUE(check);
}

TEST(third_party_tests, del)
{
    bool check = true;
    linked_ptr<std::vector<int>> vptr(new std::vector<int>{1, 2, 3, 4});
    linked_ptr<std::vector<int>> v2ptr(vptr);

    check *= !v2ptr.unique();

    vptr.~linked_ptr<std::vector<int>>();
    std::vector<int> v = *v2ptr;

    check *= (v[0] == 1);
    check *= v2ptr.unique();

    v2ptr.reset();
    check *=  (v2ptr.get() == nullptr);

    ASSERT_TRUE(check);
}

TEST(third_party_tests, reset)
{
    bool check = true;

    linked_ptr<double> ptr1(new double(3.6));
    linked_ptr<double> ptr2(new double(3.6));

    check *= (ptr1.get() != ptr2.get());

    linked_ptr<double> nptr1(ptr1);
    (*nptr1) = 4;
    check *= (*ptr1 == 4);

    nptr1.reset(new double(3.6));

    check *= (*nptr1 == 3.6);

    ASSERT_TRUE(check);
}

TEST(third_party_tests, test)
{
    linked_ptr<A> ptr(new B());
    bool check = (ptr->a == 1);
    ASSERT_TRUE(check);
}

TEST(third_party_tests, set)
{
    std::set<linked_ptr<int>> S;
    linked_ptr<int> p1(new int(1));
    linked_ptr<int> p2(new int(2));
    linked_ptr<int> p3(new int(3));
    S.insert(p1);
    S.insert(p2);
    S.insert(p3);
    SUCCEED();
}

TEST(third_party_tests, unique)
{
    bool check = true;
    linked_ptr<int> p1(new int(1));
    linked_ptr<int> p2(new int(2));
    linked_ptr<int> p3(new int(3));
    check *= (p1.unique() && p2.unique() && p3.unique());
    linked_ptr<int> p4 = p3;
    check *= !p3.unique() && !p4.unique();
    p3.reset(new int(5));
    check *= p3.unique() && p4.unique();
    ASSERT_TRUE(check);
}

namespace unique2 {
    struct A {
        A(int a1) :a(a1) {}
        int a;
    };

    struct B : A {
        B(int a1, int b1) : A(a1), b(b1) {}
        int b;
    };
}

TEST(third_party_tests, unique2)
{
    bool check = true;
    linked_ptr<unique2::B> p1(new unique2::B(2, 3));
    linked_ptr<unique2::A> p2(p1);
    check *= !p1.unique();
    check *= !p2.unique();
    ASSERT_TRUE(check);
}