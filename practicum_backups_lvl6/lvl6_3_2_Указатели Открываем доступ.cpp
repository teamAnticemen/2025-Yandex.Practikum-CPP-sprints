#include <cassert>
#include <string>
#include <iostream>

using namespace std;

template <typename T>
bool IsSameObject(T& value1, T& value2) {
    //cout << &value1 << "  "s << &value2 << endl;
    if(&value1 == &value2) return true;
    return false;
}

int main() {
    using namespace std;

    const int x = 1;
    const int y = 1;
    // x и y Ч разные объекты, хоть и имеющие одинаковое значение
    assert(!IsSameObject(x, y));
    // ќба аргумента Ч один и тот же объект
    assert(IsSameObject(x, x));

    const string name1 = "Harry"s;
    const string name1_copy = name1;
    const string name2 = "Ronald"s;
    auto name1_ptr = &name1;
    const string& name1_ref = name1;

    assert(!IsSameObject(name1, name2));  // ƒве строки с разными значени€ми Ч разные объекты
    assert(!IsSameObject(name1, name1_copy));  // —трока и еЄ копи€ Ч разные объекты

    // ќба параметра ссылаютс€ на одну и ту же строку
    assert(IsSameObject(name1, name1));
    assert(IsSameObject(name2, name2));
    assert(IsSameObject(name1_copy, name1_copy));

    // –азыменованный указатель на объект и сам объект Ч один и тот же объект
    assert(IsSameObject(*name1_ptr, name1));

    // ѕеременна€ и ссылка на неЄ относ€тс€ к одному и тому же объекту
    assert(IsSameObject(name1_ref, name1));

    // —сылка на объект и разыменованный указатель на объект относ€тс€ к одному и тому же объекту
    assert(IsSameObject(name1_ref, *name1_ptr));
}