#include <cassert>
#include <string>
#include <string_view>
#include <utility>

using namespace std::literals;

template <typename T, typename... Args>
bool EqualsToOneOf(T&& value, Args&&... args) {
    return ((value == std::forward<Args>(args)) || ...);
}

int main() {
    assert(EqualsToOneOf("hello"sv, "hi"s, "hello"s));
    assert(!EqualsToOneOf(1, 10, 2, 3, 6));
    assert(!EqualsToOneOf(8));
}