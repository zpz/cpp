#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <typeinfo>

using namespace std;


class A
{
  public:
    virtual char name() const
    {
        return 'A';
    }
};

class B : public A
{
  public:
    virtual char name() const
    {
        return 'B';
    }
};


int main()
{
    unique_ptr<A> a1 = make_unique<A>();
    unique_ptr<B> b1 = make_unique<B>();
    unique_ptr<B> b2 = make_unique<B>();
    unique_ptr<B> b3 = make_unique<B>();
    unique_ptr<A> ab1 = std::unique_ptr<A>(new B());

    assert(typeid(*a1) != typeid(*b1));
    assert(typeid(*b1) == typeid(*ab1));
    assert(typeid(*b1.get()) == typeid(*ab1));
    assert(typeid(*b1) == typeid(*ab1.get()));

    unique_ptr<A> abb(new B());

    assert(typeid(*b1) == typeid(*abb));

    unique_ptr<A> ab1_m(move(b1));

    assert(ab1_m->name() == 'B');
    assert(typeid(*ab1_m) == typeid(*b2));
    assert(typeid(*ab1_m) != typeid(*a1));

    unique_ptr<A> ab2_m(b2.release());
    assert(typeid(*ab2_m) == typeid(*b3));
    assert(typeid(*ab2_m) != typeid(*a1));
}

