#include <cassert>
#include <cstddef>
#include <string>
#include <utility>

template <typename Type>
class SingleLinkedList {
    // ���� ������
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val), next_node(next) {
        }
        Type value;
        Node* next_node = nullptr;
    };

public:
    // ����������� �� ���������
    SingleLinkedList() : head_(Node()), size_(0) {}

    // ���������� ���������� ��������� � ������
    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    // ��������, ������ �� ������
    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0;
    }

private:
    // ��������� ����, ������������ ��� ������� "����� ������ ���������"
    Node head_;
    size_t size_;
};

void Test0() {
    using namespace std;
    {
        const SingleLinkedList<int> empty_int_list;
        assert(empty_int_list.GetSize() == 0u);
        assert(empty_int_list.IsEmpty());
    }

    {
        const SingleLinkedList<string> empty_string_list;
        assert(empty_string_list.GetSize() == 0u);
        assert(empty_string_list.IsEmpty());
    }
}

int main() {
    Test0();
}