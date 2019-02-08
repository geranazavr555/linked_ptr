#include "gtest.h"
#include "linked_ptr.h"

TEST(constructors, default_)
{
    linked_ptr<int> i;
    linked_ptr<double> d;

    linked_ptr< linked_ptr<bool> > llb;
}

class ForwardDeclaratedClass;

TEST(constructors, forward_decl)
{
    linked_ptr<ForwardDeclaratedClass> x;
}