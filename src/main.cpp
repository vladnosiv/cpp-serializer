#include <cassert>
#include <iostream>
#include <list>
#include <set>
#include <vector>

#include "serializer.hpp"

class A {
public:
    int a;
    bool b;
    std::vector<double> v;

    bool operator==(const A &obj) const {
        return a == obj.a && b == obj.b && v == obj.v;
    }

    SERIALIZE_CLASS(a, b, v)
};

int main() {
    std::list<std::set<int>> s = {
            {1, 3},
            {3, 67},
            {3, 25, 345, 456, 4},
            {4, 2},
            {5}
    };
    serializer::save_to_file("tmp.txt", s);
    auto s_new = serializer::read_from_file<decltype(s)>("tmp.txt");
    assert(s == s_new);

    A a{1337, false, {1.1, 6.43, 874.23}};
    serializer::save_to_file("tmp.txt", a);
    auto new_a = serializer::read_from_file<A>("tmp.txt");
    assert(a == new_a);
}
