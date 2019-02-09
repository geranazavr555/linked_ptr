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
/*
TEST(constructors, forward_decl_def)
{
    // Must compile
    linked_ptr<ForwardDeclaratedClass> x;
    //extern ForwardDeclaratedClass exfwd;
    //std::shared_ptr<ForwardDeclaratedClass> x(&exfwd);
    std::shared_ptr<ForwardDeclaratedClass> y;
    //std::shared_ptr<int> z(new int(5));
    //std::cout << "kek";
    GTEST_FAIL();
}
*/
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