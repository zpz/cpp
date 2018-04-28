#include <cassert>
#include <typeinfo>
#include <typeindex>
#include <iostream>
#include <memory>
#include <string>

using namespace std;


class A
{
  private:
    int _a;

  protected:
    virtual bool _equals(A const & other) const
    {
        if (typeid(*this) != typeid(other)) {
            return false;
        }
        return this->_a == other._a;
    }

  public:
    A(int a) : _a{a} {}

    bool operator==(A const & other) const
    {
        return this->_equals(other);
    }

    bool operator!=(A const & other) const
    {
        return !(*this == other);
    }
};


class B : public A
{
  private:
    int _b;

  protected:
    virtual bool _equals(A const & other) const
    {
        if (typeid(*this) != typeid(other)) {
            return false;
        }
        auto that = static_cast<B const &>(other);
        if (this->_b != that._b) {
            return false;
        }
        return A::_equals(other);
    }

  public:
    B(int a, int b) : A(a), _b{b} {}
};


class C : public A
{
  private:
    int _c;

  protected:
    virtual bool _equals(A const & other) const
    {
        if (typeid(*this) != typeid(other)) {
            return false;
        }
        auto that = static_cast<C const &>(other);
        if (this->_c != that._c) {
            return false;
        }
        return A::_equals(other);
    }

  public:
    C(int a, int c): A(a), _c{c} {}
};


class D : public C
{
  private:
    int _d;

  protected:
    virtual bool _equals(A const & other) const
    {
        if (typeid(*this) != typeid(other)) {
            return false;
        }
        auto that = static_cast<D const &>(other);
        if (this->_d != that._d) {
            return false;
        }
        return C::_equals(other);
    }

  public:
    D(int a, int c, int d): C(a, c), _d{d} {}
};


class E: public D
{
  public:
    E(int a, int c, int d) : D(a, c, d) {}
};


class F : public E
{
  private:
    int _f;

  protected:
    virtual bool _equals(A const & other) const
    {
        if (typeid(*this) != typeid(other)) {
            return false;
        }
        auto that = static_cast<F const &>(other);
        if (this->_f != that._f) {
            return false;
        }
        return E::_equals(other);
    }

  public:
    F(int a, int c, int d, int f): E(a, c, d), _f{f} {}
};




int main()
{
    A a1(2);
    A a2(2);
    A a3(3);
    B b1{2, 3};
    B b2{2, 3};
    B b3{3, 3};
    B b4(3, 4);
    C c1{8, 4};
    C c2{8, 4};
    C c3{7, 4};
    C c4{7, 5};
    D d1(1, 2, 3);
    D d2(1, 2, 3);
    D d3(1, 2, 4);
    D d4(2, 2, 4);
    D d5(1, 3, 3);
    F f1(1, 2, 3, 4);
    F f2(1, 2, 3, 4);
    F f3(1, 3, 3, 4);
    F f4(1, 2, 3, 4);

    assert(a1 == a2);
    assert(a2 != a3);
    assert(b1 == b2);
    assert(b1 != b3);
    assert(b3 != b4);
    assert(c1 == c2);
    assert(c1 != c3);
    assert(c3 != c4);
    assert(d1 == d2);
    assert(d1 != d3);
    assert(d3 != d4);
    assert(d1 != d5);
    assert(f1 == f2);
    assert(f2 != f3);
    assert(f3 != f4);

    assert(a1 != b2);
    assert(b3 != a2);
    assert(a2 != c2);
    assert(c3 != a1);
    assert(a2 != d1);
    assert(d5 != a1);
    assert(b2 != c3);
    assert(c3 != b1);
    assert(c4 != d1);
    assert(d3 != c2);
    assert(d2 != f1);
    assert(f2 != c3);

    assert(typeid(a1) == typeid(a2));
    assert(typeid(b1) == typeid(b3));
    assert(typeid(c3) == typeid(c4));
    assert(typeid(d1) == typeid(d4));
    assert(typeid(f1) == typeid(f2));
    assert(typeid(a1) != typeid(b1));
    assert(typeid(b1) != typeid(c1));
    assert(typeid(c1) != typeid(d1));
    assert(typeid(d1) != typeid(f3));

    assert(type_index(typeid(a1)) == type_index(typeid(a3)));
    assert(type_index(typeid(b1)) == type_index(typeid(b3)));
    assert(type_index(typeid(c1)) == type_index(typeid(c3)));
    assert(type_index(typeid(d3)) == type_index(typeid(d4)));
    assert(type_index(typeid(a1)) != type_index(typeid(c1)));

    assert(type_index(typeid(a2)).hash_code() == type_index(typeid(a3)).hash_code());
    assert(type_index(typeid(b1)).hash_code() == type_index(typeid(b3)).hash_code());
    assert(type_index(typeid(c1)).hash_code() == type_index(typeid(c3)).hash_code());
    assert(type_index(typeid(d2)).hash_code() == type_index(typeid(d3)).hash_code());
    assert(type_index(typeid(b2)).hash_code() != type_index(typeid(c2)).hash_code());

    A* aa1 = &a1;
    A* aa2 = &a2;
    A* aa3 = &a3;
    A* ab1 = &b1;
    A* ab2 = &b2;
    A* ab3 = &b3;
    A* ab4 = &b4;
    A* ac1 = &c1;
    A* ac2 = &c2;
    A* ac3 = &c3;
    A* ac4 = &c4;
    A* ad1 = &d1;
    A* ad2 = &d2;
    A* ad3 = &d3;
    A* ad4 = &d4;
    A* ad5 = &d5;
    A* af1 = &f1;
    A* af2 = &f2;
    A* af3 = &f3;
    A* af4 = &f4;

    assert(*aa1 == *aa2);
    assert(*aa2 != *aa3);
    assert(*ab1 == *ab2);
    assert(*ab1 != *ab3);
    assert(*ab3 != *ab4);
    assert(*ac1 == *ac2);
    assert(*ac1 != *ac3);
    assert(*ac3 != *ac4);
    assert(*ad1 == *ad2);
    assert(*ad1 != *ad3);
    assert(*ad3 != *ad4);
    assert(*ad1 != *ad5);
    assert(*af1 == *af2);
    assert(*af2 != *af3);
    assert(*af3 != *af4);

    assert(typeid(*aa1) == typeid(*aa2));
    assert(typeid(*ab1) == typeid(*ab3));
    assert(typeid(*ac3) == typeid(*ac4));
    assert(typeid(*ad1) == typeid(*ad4));
    assert(typeid(*af1) == typeid(*af2));
    assert(typeid(*aa1) != typeid(*ab1));
    assert(typeid(*ab1) != typeid(*ac1));
    assert(typeid(*ac1) != typeid(*ad1));
    assert(typeid(*ad1) != typeid(*af3));

    assert(type_index(typeid(*aa1)) == type_index(typeid(*aa3)));
    assert(type_index(typeid(*ab1)) == type_index(typeid(*ab3)));
    assert(type_index(typeid(*ac1)) == type_index(typeid(*ac3)));
    assert(type_index(typeid(*ad3)) == type_index(typeid(*ad4)));
    assert(type_index(typeid(*aa1)) != type_index(typeid(*ac1)));

    assert(type_index(typeid(*aa2)).hash_code() == type_index(typeid(*aa3)).hash_code());
    assert(type_index(typeid(*ab1)).hash_code() == type_index(typeid(*ab3)).hash_code());
    assert(type_index(typeid(*ac1)).hash_code() == type_index(typeid(*ac3)).hash_code());
    assert(type_index(typeid(*ad2)).hash_code() == type_index(typeid(*ad3)).hash_code());
    assert(type_index(typeid(*ab2)).hash_code() != type_index(typeid(*ac2)).hash_code());

    unique_ptr<A> uaa1(new A(move(a1)));
    unique_ptr<A> uab1(new B(move(b1)));
    unique_ptr<A> uab2(new B(move(b2)));
    unique_ptr<A> uac1(new C(move(c1)));

    assert(*uab1 == *uab2);
    assert(*uaa1 != *uab2);
    assert(*uab1 != *uac1);

    assert(typeid(*uab1) != typeid(*uac1));
    assert(typeid(*uab1) == typeid(*uab2));
}
