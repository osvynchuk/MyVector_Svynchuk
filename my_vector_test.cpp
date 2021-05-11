// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "gtest/gtest.h"
#include "my_vector.h"
#include <exception>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <list>

using namespace cpp_training;

template <typename T>
std::ostream& operator << (std::ostream& os, const my_vector<T>& co) {
    os << "{";
    for (auto const& i : co) { os << ' ' << i; }
    return os << " } ";
}

//
// A simple type for container testing purposes
//
class Foo {
public:
    Foo() { std::cout << "Foo()" << std::endl;}
    Foo(float v) : val(v) { std::cout << "Foo(float)" << std::endl;}
    Foo(const Foo& v) : val(v.val) { std::cout << "Foo(cons Foo&)" << std::endl; }
    Foo(Foo&& v) : val(v.val) { v.val = 0; std::cout << "Foo(Foo&&)" << std::endl; }
    ~Foo() { std::cout << "~Foo()" << std::endl;}

    bool operator == (const Foo& rhs) const { return val == rhs.val; }
    bool operator < (const Foo& rhs) const { return val < rhs.val; }
    bool operator <= (const Foo& rhs) const { return val <= rhs.val; }
    bool operator > (const Foo& rhs) const { return val > rhs.val; }
    bool operator >= (const Foo& rhs) const { return val >= rhs.val; }

    Foo& operator = (const Foo& rhs) {
        val = rhs.val;
        std::cout << "Foo::operator = (Foo&)" << std::endl;
        return *this;
    }
    Foo& operator = (Foo&& rhs) {
        val = rhs.val;
        rhs.val = 0;
        std::cout << "Foo::operator = (Foo&&)" << std::endl;
        return *this;
    }
    friend std::ostream& operator << (std::ostream& os, const Foo& f) {
        std::cout << "Foo{" << f.val << "} ";
        return os;
    }

    void foo() {}
private:
    float val = 0;
};


TEST(MyVectorTest, Construction) {
    my_vector<int> vect;
    my_vector<Foo> foo_vec_empt;
    my_vector<Foo> foo_vec(3);

   EXPECT_THROW(vect.at(0), std::out_of_range);
   EXPECT_EQ(foo_vec_empt.size(), 0);
   EXPECT_EQ(foo_vec_empt.capacity(), 0);
   EXPECT_EQ(foo_vec.size(), 3);
   EXPECT_EQ(foo_vec.capacity(), 4);

   Foo foo(3);
   my_vector<Foo> foo_vec2(3, foo);
   EXPECT_EQ(foo_vec2[0], foo);
   EXPECT_EQ(foo_vec2[1], foo);
   EXPECT_EQ(foo_vec2[2], foo);

   foo_vec2[1] = Foo{123};
   foo_vec2[2] = Foo{456};

   my_vector<Foo> foo_vec3(foo_vec2);
   EXPECT_EQ(foo_vec3.size(), foo_vec2.size());
   EXPECT_EQ(foo_vec3.capacity(), foo_vec2.capacity());
   EXPECT_EQ(foo_vec3[0], foo_vec2[0]);
   EXPECT_EQ(foo_vec3[1], foo_vec2[1]);
   EXPECT_EQ(foo_vec3[2], foo_vec2[2]);

   // Move constructor
   my_vector<std::string> src {"abc", "def"};
   my_vector<std::string> vec4(std::move(src));
   EXPECT_EQ(vec4.size(), 2);
   EXPECT_EQ(src.size(), 0);
   EXPECT_EQ(vec4[0], "abc");
   EXPECT_EQ(vec4[1], "def");

   auto lmda = [] () {
       return my_vector<std::string>{"a","b","c"};
   };

   my_vector<std::string> vec5(lmda());
   EXPECT_EQ(vec5.size(), 3);
   EXPECT_EQ(vec5[0], "a");
   EXPECT_EQ(vec5[1], "b");
   EXPECT_EQ(vec5[2], "c");
}

TEST(MyVectorTest, ConstructionFromRange) {
//    int arr[] = {1,2,3,4,5};
//    my_vector<int> vect {*(arr+1), *(arr + sizeof (arr)/sizeof(arr[0]) - 1)};
//    EXPECT_EQ(vect.size(), 3);
//    EXPECT_EQ(vect[0], 2);
//    EXPECT_EQ(vect[1], 3);
//    EXPECT_EQ(vect[2], 4);

    //my_vector(InIter begin, InIter end)
    std::vector<int> cont {6,7,8,9,10};
    my_vector<int> vect2 {cont.begin()+1, cont.end() - 1};
    EXPECT_EQ(vect2.size(), 3);
    EXPECT_EQ(vect2[0], 7);
    EXPECT_EQ(vect2[1], 8);
    EXPECT_EQ(vect2[2], 9);

    std::list<std::string> list {"brown", "fox", "jumps"};
    my_vector<std::string> vectr {list.begin(), list.end()};
    EXPECT_EQ(vectr.size(), 3);
    EXPECT_EQ(vectr[0], "brown");
    EXPECT_EQ(vectr[1], "fox");
    EXPECT_EQ(vectr[2], "jumps");

    std::string arr_s[] = {"hello", "world", "iter", "test"};
    my_vector<std::string> vect ( arr_s, arr_s + 4 );
    EXPECT_EQ(vect.size(), 4);
    EXPECT_EQ(vect[0], "hello");
    EXPECT_EQ(vect[1], "world");
    EXPECT_EQ(vect[2], "iter");

    //my_vector(std::initializer_list)
    my_vector<int> vect3 { 1, 3, 5 };
    EXPECT_EQ(vect3.size(), 3);
    EXPECT_EQ(vect3[0], 1);
    EXPECT_EQ(vect3[1], 3);
    EXPECT_EQ(vect3[2], 5);

    my_vector<Foo> foo_vec {3, 2, 5};
    my_vector<Foo> vect4 = foo_vec;
    EXPECT_EQ(vect4.size(), 3);
    EXPECT_EQ(vect4[0], Foo{3});
    EXPECT_EQ(vect4[1], Foo{2});
    EXPECT_EQ(vect4[2], Foo{5});

    std::cout << std::endl;
    my_vector<Foo> vect5 (std::move(foo_vec));
    EXPECT_EQ(vect5.size(), 3);
    EXPECT_EQ(foo_vec.size(), 0);
    EXPECT_EQ(vect5[0], Foo{3});
    EXPECT_EQ(vect5[1], Foo{2});
    EXPECT_EQ(vect5[2], Foo{5});

    my_vector<int> vect6 { vect3.begin(), vect3.end() };
    EXPECT_EQ(vect6.size(), 3);
    EXPECT_EQ(vect6[0], 1);
    EXPECT_EQ(vect6[1], 3);
    EXPECT_EQ(vect6[2], 5);
}

TEST(MyVectorTest, Assign) {
   my_vector<Foo> foo_vec_src {7,8,9};
   my_vector<Foo> vec_dest {1,2};

   // Call to my_vector& operator = (Cont& rhs)
   vec_dest = foo_vec_src;
   EXPECT_EQ(vec_dest.size(), 3);
   EXPECT_EQ(foo_vec_src.size(), 3);
   EXPECT_EQ(vec_dest[0], Foo{7});
   EXPECT_EQ(vec_dest[1], Foo{8});
   EXPECT_EQ(vec_dest[2], Foo{9});

   // Call to my_vector& operator = (Cont&& rhs) - MOVE op
    my_vector<Foo> vec_dest2 {1,2,3};
    vec_dest2 = std::move(foo_vec_src);

    EXPECT_EQ(vec_dest2.size(), 3);
    EXPECT_EQ(foo_vec_src.size(), 0);
    EXPECT_EQ(foo_vec_src.capacity(), 0);
    EXPECT_EQ(vec_dest2[0], Foo(7));
    EXPECT_EQ(vec_dest2[1], Foo(8));
    EXPECT_EQ(vec_dest2[2], Foo(9));

    {
        auto ret_byval = [] () -> my_vector<Foo> {
            my_vector<Foo> tmp {2,3,4};
            return tmp;
        };

        // Call my_vector& operator = (Cont&& rhs) with MOVE
        my_vector<Foo> vec_dest {1,2};
        vec_dest = ret_byval();
        EXPECT_EQ(vec_dest.size(), 3);
        EXPECT_EQ(vec_dest[0], Foo{2});
        EXPECT_EQ(vec_dest[1], Foo{3});
        EXPECT_EQ(vec_dest[2], Foo{4});
    }

    // Assign temp vector
    // call to my_vector& operator = (const my_vector& rhs)
    // vector& operator=( std::initializer_list<T> ilist );
    {
        my_vector<Foo> vec_dest {1,2};
        vec_dest = {1.0, 2.1, 3};
        EXPECT_EQ(vec_dest.size(), 3);
        EXPECT_EQ(vec_dest[0], Foo{1});
        EXPECT_EQ(vec_dest[1], Foo{2.1});
        EXPECT_EQ(vec_dest[2], Foo{3});
    }

    // Assign const vector
    // call to my_vector& operator = (const my_vector& rhs)
    {
        my_vector<Foo> vec_dest {1,2};
        const my_vector<Foo> foo_vec {7,8,9};
        vec_dest = foo_vec;
        EXPECT_EQ(vec_dest.size(), 3);
        EXPECT_EQ(vec_dest[0], Foo{7});
        EXPECT_EQ(vec_dest[1], Foo{8});
        EXPECT_EQ(vec_dest[2], Foo{9});
    }
}


TEST(MyVectorTest, Reserve) {
    my_vector<int> vect;

   EXPECT_EQ(vect.size(), 0);
   EXPECT_EQ(vect.capacity(), 0);

   vect.reserve(10);
   EXPECT_EQ(vect.size(), 0);
   EXPECT_EQ(vect.capacity(), 10);

   my_vector<int> vec {1, 2, 4, 8, 11};
   vec.reserve(15);

   EXPECT_EQ(vec[1], 2);
   EXPECT_EQ(vec[3], 8);
   EXPECT_EQ(vec[4], 11);
}

TEST(MyVectorTest, Indexation) {
   my_vector<int> vec {1, 2, 4, 6};
   EXPECT_EQ(vec[1], 2);
   EXPECT_EQ(vec.at(2), 4);
   EXPECT_THROW(vec.at(5), std::out_of_range);
}

TEST(MyVectorTest, PushBack) {
    my_vector<double> cont;

    cont.push_back(1);
    EXPECT_EQ(cont.size(), 1);
    EXPECT_EQ(cont.capacity(), 1);

    cont.push_back(2);
    EXPECT_EQ(cont.size(), 2);
    EXPECT_EQ(cont.capacity(), 3);

    cont.push_back(3);
    EXPECT_EQ(cont.size(), 3);
    EXPECT_EQ(cont.capacity(), 3);

    EXPECT_EQ(cont[0], 1);
    EXPECT_EQ(cont[1], 2);
    EXPECT_EQ(cont[2], 3);
}

TEST(MyVectorTest, PushBack2) {
    const Foo foo;
    my_vector<Foo> c;

    c.push_back(foo); // reallocation happened
    c.push_back(foo);
    c.push_back(foo);
    c.push_back(foo);
    c.push_back(foo);

    std::cout << std::endl;
    my_vector<Foo> cont;
    cont.reserve(5);

    cont.push_back(foo);
    cont.push_back(foo);

    // Moving push back version
    cont.push_back(Foo{345});
    EXPECT_EQ(cont[2], Foo{345});
}

TEST(MyVectorTest, PopBack) {
    my_vector<Foo> empty;
    empty.pop_back();

    my_vector<Foo> c {2,3,4};
    c.pop_back();
    std::cout << c << std::endl;
    EXPECT_EQ(c.size(), 2);
    EXPECT_EQ(c[0], 2);
    EXPECT_EQ(c[1], 3);

    my_vector<int> numbers;
    numbers.push_back(5);
    numbers.push_back(3);
    numbers.push_back(4);
    numbers.pop_back();
    std::cout << numbers << std::endl;
}

TEST(MyVectorTest, Swap) {
    my_vector<int> a1{1, 2, 3}, a2{4, 5};

    auto it1 = std::next(a1.begin());
    auto it2 = std::next(a2.begin());

    int& ref1 = a1.front();
    int& ref2 = a2.front();

    std::cout << a1 << a2 << *it1 << ' ' << *it2 << ' ' << ref1 << ' ' << ref2 << '\n';
    a1.swap(a2);
    std::cout << a1 << a2 << *it1 << ' ' << *it2 << ' ' << ref1 << ' ' << ref2 << '\n';

    // Note that after swap the iterators and references stay associated with their
    // original elements, e.g. it1 that pointed to an element in 'a1' with value 2
    // still points to the same element, though this element was moved into 'a2'.
}

TEST(MyVectorTest, Clear) {
    my_vector<int> container{1, 2, 3};

    auto print = [](const int& n) { std::cout << " " << n; };

    std::cout << "Before clear:";
    std::for_each(container.begin(), container.end(), print);
    std::cout << "\nSize=" << container.size() << '\n';
    EXPECT_EQ(container.size(), 3);

    std::cout << "Clear\n";
    container.clear();

    std::cout << "After clear:";
    std::for_each(container.begin(), container.end(), print);
    std::cout << "\nSize=" << container.size() << '\n';

    EXPECT_EQ(container.size(), 0);
}

TEST(MyVectorTest, Resize) {
    my_vector<int> c = {1, 2, 3};
    std::cout << "The vector holds: ";
    for(auto& el: c) std::cout << el << ' ';
    std::cout << '\n';
    c.resize(5);
    EXPECT_EQ(c.size(), 5);
    std::cout << "After resize up to 5: ";
    for(auto& el: c) std::cout << el << ' ';
    std::cout << '\n';
    c.resize(2);
    EXPECT_EQ(c.size(), 2);
    std::cout << "After resize down to 2: ";
    for(auto& el: c) std::cout << el << ' ';
    std::cout << '\n';
}

TEST(MyVectorTest, Resize2) {
    my_vector<Foo> cont = {Foo{1}, Foo{2}, Foo{3}};
    cont.resize(5);
    std::cout << std::endl;
    cont.resize(2);
    std::cout << std::endl;
}

TEST(MyVectorTest, Shrink) {
    my_vector<int> v;
    std::cout << "Default-constructed capacity is " << v.capacity() << '\n';
    v.resize(100);
    std::cout << "Capacity of a 100-element vector is " << v.capacity() << '\n';
    EXPECT_EQ(v.capacity(), 100 * my_vector<int>::CapacityFactor);

    v.resize(50);
    std::cout << "Capacity after resize(50) is " << v.capacity() << '\n';
    EXPECT_EQ(v.capacity(), 100 * my_vector<int>::CapacityFactor);

    v.shrink_to_fit();
    std::cout << "Capacity after shrink_to_fit() is " << v.capacity() << '\n';
    EXPECT_EQ(v.capacity(), 50);

    v.clear();
    std::cout << "Capacity after clear() is " << v.capacity() << '\n';
    EXPECT_EQ(v.capacity(), 50);

    v.shrink_to_fit();
    std::cout << "Capacity after shrink_to_fit() is " << v.capacity() << '\n';
    EXPECT_EQ(v.capacity(), 0);

    for (int i = 1000; i < 1300; ++i) {
        v.push_back(i);
    }
    std::cout << "Capacity after adding 300 elements is " << v.capacity() << '\n';
    EXPECT_EQ(v.capacity(), 313);

    v.shrink_to_fit();
    std::cout << "Capacity after shrink_to_fit() is " << v.capacity() << '\n';
    EXPECT_EQ(v.capacity(), 300);
}

TEST(MyVectorTest, FrontBack) {
    my_vector<int> vec {1, 2, 4, 6};
    EXPECT_EQ(vec.front(), 1);
    EXPECT_EQ(vec.back(), 6);

    my_vector<std::string> vec2 {"1", "4", "6"};
    EXPECT_EQ(vec2.front(), "1");
    EXPECT_EQ(vec2.back(), "6");
}

TEST(MyVectorTest, Iterators) {
    my_vector<int> vec {1, 2, 4, 6};
    EXPECT_EQ(*vec.begin(), 1);
    EXPECT_EQ(*(vec.end()-1), 6);
    EXPECT_EQ(*(vec.begin()+1), 2);

    auto iter = vec.begin();

    // []
    EXPECT_EQ(iter[1], 2);

    // Inc, dec
    EXPECT_EQ(*iter++, 1);
    EXPECT_EQ(*iter, 2);
    EXPECT_EQ(*iter--, 2);
    EXPECT_EQ(*iter, 1);

    iter = vec.begin();
    EXPECT_EQ(*++iter, 2);
    EXPECT_EQ(*--iter, 1);

    // +, -, +=
    EXPECT_EQ(*(iter + 2), 4);
    EXPECT_EQ(*(iter += 2), 4);
    EXPECT_EQ(*(iter - 2), 1);
    EXPECT_EQ(*(iter -= 2), 1);

    // Const
    auto citer = vec.cbegin();
    EXPECT_EQ(*citer, 1);
    //*citer = 22; // Error

    citer = vec.cend();
    EXPECT_EQ(*(citer - 1), 6);

    // for each
    my_vector<std::string> cont {"ab", "cd", "ef"};
    for (auto& str : cont) {
        std::cout << str << std::endl;
    }

    // ->
    my_vector<Foo> c {Foo(42)};
    c.begin()->foo();
}

TEST(MyVectorTest, ReverseIterators) {
    //
    // Reverse iterators
    //
    my_vector<int> vec2 {1, 2, 3, 5};
    EXPECT_EQ(*vec2.rbegin(), 5);
    EXPECT_EQ(*(vec2.rbegin()+1), 3);
    EXPECT_EQ(*(vec2.rend()-1), 1);
    EXPECT_EQ(*(vec2.rend()-2), 2);

    EXPECT_EQ(*vec2.rcbegin(), 5);
    EXPECT_EQ(*(vec2.rcbegin()+1), 3);
    EXPECT_EQ(*(vec2.rcend()-1), 1);
    EXPECT_EQ(*(vec2.rcend()-2), 2);
}

TEST(MyVectorTest, Insert) {
    my_vector<int> cnt;
    auto pos0 = cnt.insert(cnt.begin(), 677);
    EXPECT_EQ(cnt.size(), 1);
    EXPECT_EQ(cnt[0], 677);
    EXPECT_EQ(cnt.begin(), pos0);

    my_vector<int> c {1,2,4,5};
    EXPECT_EQ(c.size(), 4);

    auto pos = c.insert(c.cend(), 6);
    EXPECT_EQ(c.size(), 5);
    EXPECT_EQ(c[4], 6);
    EXPECT_EQ(pos, c.end() - 1);
    EXPECT_EQ(*pos, 6);
    EXPECT_EQ(c, (my_vector<int>{1,2,4,5, 6}));
    std::cout << c << std::endl;

    c.insert(c.cbegin(), 0);
    EXPECT_EQ(c.size(), 6);
    EXPECT_EQ(c[0], 0);
    EXPECT_EQ(c, (my_vector<int>{0,1,2,4,5,6}));
    std::cout << c << std::endl;

    auto it = std::find(c.begin(), c.end(), 4);
    EXPECT_EQ(*it, 4);

    c.insert(it, 3);
    EXPECT_EQ(c.size(), 7);
    EXPECT_EQ(c[3], 3);
    EXPECT_EQ(c, (my_vector<int>{0,1,2,3,4,5,6}));
    std::cout << c << std::endl;

    {
     // Copy constructors are called for init list members
     my_vector<Foo> vec {{1}, {2}};

    auto pos = vec.insert(vec.begin(), Foo{0});
    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[0], Foo{0});
    EXPECT_EQ(pos, vec.begin());
    EXPECT_EQ(*pos, Foo{0});
    std::cout << vec << std::endl;
    }
}

TEST(MyVectorTest, InsertRange) {

    my_vector<int> c {1,2,4,5};
    //std::vector<int> c {1,2,4,5};
    EXPECT_EQ(c.size(), 4);

    std::vector<int> src {9, 10, 11};
    auto pos = c.insert(c.begin(), src.begin(), src.end());
    EXPECT_EQ(c.size(), 7);
    EXPECT_EQ(*c.begin(), 9);
    EXPECT_EQ(c.back(), 5);
    EXPECT_EQ(*pos, 9);
    std::cout << c << std::endl;

    c = {1,2,4,5};
    pos = c.insert(c.end(), src.begin(), src.end());
    EXPECT_EQ(c.size(), 7);
    EXPECT_EQ(*c.begin(), 1);
    EXPECT_EQ(c.back(), 11);
    EXPECT_EQ(*pos, 9);
    std::cout << c << std::endl;

    c = {1,2,4,5};
    pos = c.insert(c.begin() + 2, src.begin(), src.end());
    EXPECT_EQ(c.size(), 7);
    EXPECT_EQ(*c.begin(), 1);
    EXPECT_EQ(c.back(), 5);
    EXPECT_EQ(*pos, 9);
    std::cout << c << std::endl;

    c = {};
    pos = c.insert(c.end(), src.begin(), src.end());
    EXPECT_EQ(c.size(), 3);
    EXPECT_EQ(*c.begin(), 9);
    EXPECT_EQ(c.back(), 11);
    EXPECT_EQ(*pos, 9);
    std::cout << c << std::endl;

    my_vector<Foo> v {{1}, {5}};
    std::vector<Foo> s {{2}, {3}};
    auto it = v.insert(v.begin(), s.begin(), s.end());
    std::cout << v << std::endl;
}

TEST(MyVectorTest, Erase) {
    my_vector<Foo> empty;
    empty.erase(empty.begin());

    my_vector<Foo> c {2,3,4};
    auto it = c.erase(c.begin());
    std::cout << c << std::endl;
    EXPECT_EQ(c.size(), 2);
    EXPECT_EQ(c[0], 3);
    EXPECT_EQ(c[1], 4);
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(it, c.begin());

    my_vector<Foo> c2 {2,3,4};
    c2.erase(c2.end());
    std::cout << c2 << std::endl;

    auto it2 = c2.erase(c2.end()-1);
    std::cout << c2 << std::endl;
    EXPECT_EQ(c2.size(), 2);
    EXPECT_EQ(c2[0], 2);
    EXPECT_EQ(c2[1], 3);
    EXPECT_EQ(it2, c2.end());

    my_vector<Foo> c3 {2,3,4};
    std::cout << "before" << std::endl;
    c3.erase(c3.end()-2);
    std::cout << "after" << std::endl;
    std::cout << c3 << std::endl;
    EXPECT_EQ(c3.size(), 2);
    EXPECT_EQ(c3[0], 2);
    EXPECT_EQ(c3[1], 4);

    my_vector<int> vi {2,3,4};
    vi.erase(vi.begin());
    std::cout << vi << std::endl;
    EXPECT_EQ(vi.size(), 2);
    EXPECT_EQ(vi[0], 3);
    EXPECT_EQ(vi[1], 4);

    my_vector<int> vi2 {2,3,4};
    vi2.erase(vi2.end()-1);
    std::cout << vi2 << std::endl;
    EXPECT_EQ(vi2.size(), 2);
    EXPECT_EQ(vi2[0], 2);
    EXPECT_EQ(vi2[1], 3);

    my_vector<int> vi3 {2,3,4};
    vi3.erase(vi3.end()-2);
    std::cout << vi3 << std::endl;
    EXPECT_EQ(vi3.size(), 2);
    EXPECT_EQ(vi3[0], 2);
    EXPECT_EQ(vi3[1], 4);

    {
        my_vector<int> c{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        // Erase all even numbers (C++11 and later)
        for (auto it = c.begin(); it != c.end(); ) {
            if (*it % 2 == 0) {
                it = c.erase(it);
            } else {
                ++it;
            }
        }
        std::cout << c << std::endl;
    }
}

TEST(MyVectorTest, EraseRange) {
    my_vector<Foo> empty;
    empty.erase(empty.begin(), empty.end()-1);

    my_vector<Foo> c {2, 3, 4, 5, 6, 7};
    auto it = c.erase(c.begin()+1, c.begin()+3);
    std::cout << c << std::endl;

    EXPECT_EQ(c.size(), 4);
    EXPECT_EQ(c[0], 2);
    EXPECT_EQ(c[1], 5);
    EXPECT_EQ(c[2], 6);
    EXPECT_EQ(c[3], 7);
    EXPECT_EQ(*it, 5);
    EXPECT_EQ(it, c.begin() + 1);

    my_vector<int> ci {2, 3, 4, 5, 6, 7};
    auto it1 = ci.erase(ci.end()-3, ci.end()-1);
    std::cout << ci << std::endl;

    EXPECT_EQ(ci.size(), 4);
    EXPECT_EQ(ci[0], 2);
    EXPECT_EQ(ci[1], 3);
    EXPECT_EQ(ci[2], 4);
    EXPECT_EQ(ci[3], 7);
    EXPECT_EQ(it1, ci.end()-1);

    {
        my_vector<int> ci {2, 3, 4, 5, 6, 7};
        auto it1 = ci.erase(ci.begin(), ci.begin()+2);
        std::cout << ci << std::endl;

        EXPECT_EQ(ci.size(), 4);
        EXPECT_EQ(ci[0], 4);
        EXPECT_EQ(ci[3], 7);
        EXPECT_EQ(it1, ci.begin());
    }
    {
        my_vector<int> ci {2, 3, 4, 5, 6, 7};
        auto it1 = ci.erase(ci.begin(), ci.end()-1);
        std::cout << ci << std::endl;

        EXPECT_EQ(ci.size(), 1);
        EXPECT_EQ(it1, ci.begin());
        EXPECT_EQ(*it1, 7);
    }
    {
        my_vector<int> ci {2, 3, 4, 5, 6, 7};
        auto it1 = ci.erase(ci.begin(), ci.end());
        std::cout << ci << std::endl;

        EXPECT_EQ(ci.size(), 0);
        EXPECT_EQ(it1, ci.begin());
        EXPECT_EQ(it1, ci.end());
    }
    {
        my_vector<int> ci {2, 3, 4, 5};
        auto it = ci.erase(ci.begin(), ci.begin());
        std::cout << ci << std::endl;
        EXPECT_EQ(ci.size(), 4);
        EXPECT_EQ(it, ci.begin());
    }
}

class Bar {
public:
    Bar () = default;
    Bar (int a, float b): ma(a), mb(b) {}
    friend std::ostream& operator << (std::ostream& os, const Bar& b) { os << b.ma << " " << b.mb << " "; return os; }
private:
    int ma;
    float mb;
};

TEST(MyVectorTest, EmplaceBack) {
    my_vector<Bar> c;
    c.emplace_back(2, 3.0f);
    c.emplace_back(3, 4.0f);

    std::cout << c << std::endl;

    struct President
    {
        std::string name;
        std::string country;
        int year;

        President(std::string p_name, std::string p_country, int p_year)
            : name(std::move(p_name)), country(std::move(p_country)), year(p_year)
        {
            std::cout << "I am being constructed.\n";
        }
        President(President&& other)
            : name(std::move(other.name)), country(std::move(other.country)), year(other.year)
        {
            std::cout << "I am being moved.\n";
        }
        President(const President& other) = default;
        President& operator=(const President& other) = default;
    };

    my_vector<President> elections;
    std::cout << "emplace_back:\n";
    elections.emplace_back("Nelson Mandela", "South Africa", 1994);

    my_vector<President> reElections;
    std::cout << "\npush_back:\n";
    reElections.push_back(President("Franklin Delano Roosevelt", "the USA", 1936));

    std::cout << "\nContents:\n";
    for (President const& president: elections) {
        std::cout << president.name << " was elected president of "
                  << president.country << " in " << president.year << ".\n";
    }
    for (President const& president: reElections) {
        std::cout << president.name << " was re-elected president of "
                  << president.country << " in " << president.year << ".\n";
    }
}

TEST(MyVectorTest, Compare1) {
    my_vector<Foo> empty;
    my_vector<Foo> vec {2,3,4};
    EXPECT_FALSE(empty == vec);
    EXPECT_TRUE(empty == empty);

    my_vector<Foo> vec2 {2,3,4};
    EXPECT_TRUE(vec == vec2);
    EXPECT_FALSE(vec != vec2);

    my_vector<int> ivec {1,2,3};
    my_vector<int> ivec2 {1,2,3};
    EXPECT_TRUE(ivec == ivec2);
    EXPECT_TRUE(ivec == ivec);
    EXPECT_FALSE(ivec != ivec2);

    ivec2.pop_back();
    EXPECT_FALSE(ivec == ivec2);
    EXPECT_TRUE(ivec != ivec2);

    ivec2.push_back(5);
    EXPECT_FALSE(ivec == ivec2);
}

TEST(MyVectorTest, Compare2) {
    my_vector<Foo> empty;
    my_vector<Foo> vec {2,3,4};
    EXPECT_TRUE(empty < vec);
    EXPECT_TRUE(empty <= vec);
    EXPECT_FALSE(empty > vec);
    EXPECT_FALSE(empty >= vec);

    my_vector<Foo> vec2 {2,3,4};
    EXPECT_FALSE(vec < vec);
    EXPECT_FALSE(vec < vec2);
    EXPECT_TRUE(vec <= vec2);
    EXPECT_FALSE(vec > vec2);
    EXPECT_TRUE(vec >= vec2);

    const my_vector<Foo> vec3 {2,3,5};
    EXPECT_TRUE(vec2 < vec3);
    EXPECT_TRUE(vec2 <= vec3);
    EXPECT_TRUE(vec3 > vec2);
    EXPECT_TRUE(vec3 >= vec2);

    const my_vector<Foo> vec4 {2,3};
    EXPECT_TRUE(vec4 < vec2);
    EXPECT_TRUE(vec4 <= vec2);
    EXPECT_TRUE(vec2 > vec4);
    EXPECT_TRUE(vec2 >= vec4);
    EXPECT_FALSE(vec4 > vec2);
    EXPECT_FALSE(vec4 >= vec2);
}

TEST(MyVectorTest, Compare3) {

    my_vector<int> alice{1, 2, 3};
    my_vector<int> bob{7, 8, 9, 10};
    my_vector<int> eve{1, 2, 3};

    std::cout << std::boolalpha;

    // Compare non equal containers
    std::cout << "alice == bob returns " << (alice == bob) << '\n';
    std::cout << "alice != bob returns " << (alice != bob) << '\n';
    std::cout << "alice <  bob returns " << (alice < bob) << '\n';
    std::cout << "alice <= bob returns " << (alice <= bob) << '\n';
    std::cout << "alice >  bob returns " << (alice > bob) << '\n';
    std::cout << "alice >= bob returns " << (alice >= bob) << '\n';

    std::cout << '\n';

    // Compare equal containers
    std::cout << "alice == eve returns " << (alice == eve) << '\n';
    std::cout << "alice != eve returns " << (alice != eve) << '\n';
    std::cout << "alice <  eve returns " << (alice < eve) << '\n';
    std::cout << "alice <= eve returns " << (alice <= eve) << '\n';
    std::cout << "alice >  eve returns " << (alice > eve) << '\n';
    std::cout << "alice >= eve returns " << (alice >= eve) << '\n';
}
