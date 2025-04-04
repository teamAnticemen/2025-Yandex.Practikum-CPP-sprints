#include <iostream>
#include <vector>
#include <cassert>
#include <stdexcept>
#include <memory>
#include <string>

using namespace std::literals;

template <typename T>
class PtrVector {
public:
    PtrVector() = default;

    // ������� ������ ���������� �� ����� �������� �� other
    PtrVector(const PtrVector& other) {
        items_.reserve(other.items_.size());

        try {
            for (auto p : other.items_) {
                auto p_copy = p ? new T(*p) : nullptr;  // new ����� ��������� ����������
                items_.push_back(p_copy);
            }
        } catch (...) {
            DeleteItems();
            throw;
        }
    }

    // �������� ������������
    PtrVector& operator=(const PtrVector& other) {
        if (this == &other) return *this; // ������ �� ����������������

        PtrVector temp(other); // ������� ��������� ������
        DeleteItems(); // ������� �������� ���������
        items_ = std::move(temp.items_); // ����������� ���������
        return *this;
    }

    ~PtrVector() {
        DeleteItems();
    }

    std::vector<T*>& GetItems() noexcept {
        return items_;
    }

    const std::vector<T*>& GetItems() const noexcept {
        return items_;
    }

private:
    void DeleteItems() noexcept {
        for (auto p : items_) {
            delete p;
        }
        items_.clear(); // ������� ������
    }

    std::vector<T*> items_;
};

// ��� ������� main ��������� ������ ������ PtrVector
int main() {
    struct CopyingSpy {
        CopyingSpy(int& copy_count, int& deletion_count)
            : copy_count_(copy_count)
            , deletion_count_(deletion_count) {
        }
        CopyingSpy(const CopyingSpy& rhs)
            : copy_count_(rhs.copy_count_)
            , deletion_count_(rhs.deletion_count_) {
            if (rhs.throw_on_copy_) {
                throw std::runtime_error("copy construction failed"s);
            }
            ++copy_count_;
        }
        ~CopyingSpy() {
            ++deletion_count_;
        }
        void ThrowOnCopy() {
            throw_on_copy_ = true;
        }

    private:
        int& copy_count_;
        int& deletion_count_;
        bool throw_on_copy_ = false;
    };

    // �������� ������������
    {
        int item0_copy_count = 0;
        int item0_deletion_count = 0;
        {
            PtrVector<CopyingSpy> v;

            v.GetItems().push_back(new CopyingSpy(item0_copy_count, item0_deletion_count));
            v.GetItems().push_back(nullptr);
            {
                PtrVector<CopyingSpy> v_copy;
                v_copy = v;
                assert(v_copy.GetItems().size() == v.GetItems().size());
                assert(v_copy.GetItems().at(0) != v.GetItems().at(0));
                assert(v_copy.GetItems().at(1) == nullptr);
                assert(item0_copy_count == 1);
                assert(item0_deletion_count == 0);
            }
            assert(item0_deletion_count == 1);
        }
        assert(item0_deletion_count == 2);
    }

    // �������� ������������ ����������������
    {
        int item0_copy_count = 0;
        int item0_deletion_count = 0;

        PtrVector<CopyingSpy> v;
        v.GetItems().push_back(new CopyingSpy(item0_copy_count, item0_deletion_count));
        CopyingSpy* first_item = v.GetItems().front();

        v = v;
        assert(v.GetItems().size() == 1);
        // ��� ���������������� ������� ������ ���� ����������� �� ��� �� �������
        assert(v.GetItems().front() == first_item);
        assert(item0_copy_count == 0);
        assert(item0_deletion_count == 0);
    }

    // �������� ����������� ������� �������� ������������ ���������� ��� ������������
    {
        int item0_copy_count = 0;
        int item0_deletion_count = 0;

        int item1_copy_count = 0;
        int item1_deletion_count = 0;

        // v ������ 2 ��������
        PtrVector<CopyingSpy> v;
        v.GetItems().push_back(new CopyingSpy(item0_copy_count, item0_deletion_count));
        v.GetItems().push_back(new CopyingSpy(item1_copy_count, item1_deletion_count));

        int other_item0_copy_count = 0;
        int other_item0_deletion_count = 0;
        // other_vector ������ 1 �������, ��� ����������� �������� ����� ��������� ����������
        PtrVector<CopyingSpy> other_vector;
        other_vector.GetItems().push_back(new CopyingSpy(other_item0_copy_count, other_item0_deletion_count));
        other_vector.GetItems().front()->ThrowOnCopy();

        // ��������� ������ ����������
        auto v_items(v.GetItems());

        try {
            v = other_vector;
            // �������� ������ ��������� ����������
            assert(false);
        } catch (const std::runtime_error&) {
        }

        // �������� ������� ������ �������� ��������
        assert(v.GetItems() == v_items);
        assert(item0_copy_count == 0);
        assert(item1_copy_count == 0);
        assert(other_item0_copy_count == 0);
    }

    // ����� ��������� �� �������� ����������
    std::cout << "All tests passed successfully!" << std::endl;
    return 0;
}