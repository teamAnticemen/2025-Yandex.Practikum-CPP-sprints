#include <iostream>
#include <initializer_list>
#include <cassert>
#include <utility>

template <typename Type>
class SingleLinkedList {
private:
    struct Node {
        Type value;
        Node* next_node;

        // ����������� �� ���������
        Node() : value(Type()), next_node(nullptr) {}

        Node(const Type& value, Node* next_node) : value(value), next_node(next_node) {}
    };

public:
    class BasicIterator {
    public:
        using pointer = Type*;
        using reference = Type&;

        BasicIterator() = default;
        explicit BasicIterator(Node* node) : node_(node) {} // �������� ��� explicit
        BasicIterator(const BasicIterator& other) noexcept : node_(other.node_) {}

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator& rhs) const noexcept {
            return node_ == rhs.node_;
        }
        [[nodiscard]] bool operator!=(const BasicIterator& rhs) const noexcept {
            return node_ != rhs.node_;
        }

        [[nodiscard]] reference operator*() const noexcept {
            assert(node_ != nullptr);
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            assert(node_ != nullptr);
            return &node_->value;
        }

        BasicIterator& operator++() noexcept {
            assert(node_ != nullptr);
            node_ = node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            BasicIterator temp = *this;
            ++(*this);
            return temp;
        }

        // ����� ��� �������� ���������� ���������
        bool IsValid() const {
        return node_ != nullptr; // �������� �������, ���� node_ �� ����� nullptr
        }
        
    private:
        Node* node_ = nullptr;
        friend class SingleLinkedList; // ���� ������ � node_
    };

    using Iterator = BasicIterator;
    using ConstIterator = BasicIterator;

    SingleLinkedList() : head_(), size_(0) {}

    SingleLinkedList(std::initializer_list<Type> values) : SingleLinkedList() {
        for (const auto& value : values) {
            PushBack(value);
        }
    }

    SingleLinkedList(const SingleLinkedList& other) {
        if (other.IsEmpty()) {
            head_.next_node = nullptr; // �� ��������� ���� ��������� ����� nullptr
            size_ = 0;
            return; // ������, ��� ��� ������ ����
        }
        FillFrom(other.begin(), other.end()); // ���������� ����� ����� FillFrom
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if (this == &rhs) return *this;

        SingleLinkedList temp(rhs);
        swap(temp);
        return *this;
    }

    void swap(SingleLinkedList& other) noexcept {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }

    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // ����� ��� ���������� ������ �� ������ ���� ����������
    template <typename Iterator>
    void FillFrom(Iterator begin, Iterator end) {
        Clear(); // ������������, ��� ����� Clear ���������� ��� ������� ������

        while (begin != end) {
            Add(*begin); // ��������� ������� �� ���������, ����������� ������� ������ Add
            ++begin;     // ��������� � ���������� ��������
        }
    }
    
    void Add(const Type& value) {
    Node* new_node = new Node(value, nullptr); // �������� nullptr ��� ��������� ����
    Node* tail = &head_; // �����������, ��� head_ ��� "����������" ���� (dummy node)
    while (tail->next_node != nullptr) {
        tail = tail->next_node; // ������� ����� ������
    }
    tail->next_node = new_node; // ������������ ����� ����
    size_++; // ����������� ������
    }
    
    void PushFront(const Type& value) {
        Node* new_node = new Node(value, head_.next_node);
        head_.next_node = new_node;
        ++size_;
    }

    void PushBack(const Type& value) {
        Node* new_node = new Node(value, nullptr);
        if (head_.next_node == nullptr) {
            head_.next_node = new_node;
        } else {
            Node* current = head_.next_node;
            while (current->next_node != nullptr) {
                current = current->next_node;
            }
            current->next_node = new_node;
        }
        ++size_;
    }

    void Clear() noexcept {
        Node* current = head_.next_node;
        while (current != nullptr) {
            Node* temp = current;
            current = current->next_node;
            delete temp;
        }
        head_.next_node = nullptr;
        size_ = 0;
    }

    ~SingleLinkedList() {
        Clear();
    }

    [[nodiscard]] Iterator begin() noexcept {
        return Iterator(head_.next_node);
    }

    [[nodiscard]] Iterator end() noexcept {
        return Iterator(nullptr);
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return ConstIterator(head_.next_node);
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return ConstIterator(nullptr);
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return begin();
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return end();
    }

    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator(&head_);
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator(const_cast<Node*>(&head_));
    }

    void PopFront() noexcept {
        assert(!IsEmpty());  // ���������, ��� ������ �� ����
            Node* to_delete = head_.next_node;
            head_.next_node = head_.next_node->next_node;
            delete to_delete;
            --size_;        
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        assert(pos.IsValid()); // (��������) ���������, ��� �������� �������
        Node* new_node = new Node(value, pos.node_->next_node);
        pos.node_->next_node = new_node;
        ++size_;
        return Iterator(new_node);
    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
        assert(pos.node_ != nullptr);
        Node* to_delete = pos.node_->next_node;
        if (to_delete == nullptr) return Iterator(nullptr);

        pos.node_->next_node = to_delete->next_node;
        delete to_delete;
        --size_;
        return Iterator(pos.node_->next_node);
    }
    
    size_t size() const { return size_; }
    
private:
    Node head_; // ��������� ����
    size_t size_;
};

// �������� ��������� � swap
template <typename Type> 
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    // ���������� �������
    if (lhs.size() != rhs.size()) {
        return false; // ���� ������� �� ���������, ������ ��������
    }

    // ������ ���������� �������� � ������� std::equal
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
// ���������� std::lexicographical_compare ��� ��������� ���� �������
return std::lexicographical_compare(lhs.begin(), lhs.end(),rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}