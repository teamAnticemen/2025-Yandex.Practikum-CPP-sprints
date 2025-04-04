#include <cassert>

// ����� ���������, ��������� ��������� ������ ��� ����� ����������.
// �������� ������� T ������ ��� �������, �� ������� ��������� ���������
template <typename T>
class ScopedPtr {
public:
    // ����������� �� ��������� ������� ������� ���������,
    // ��� ��� ���� ptr_ ����� �������� �� ��������� nullptr
    ScopedPtr() = default;

    // ������� ���������, ����������� �� ���������� raw_ptr.
    // raw_ptr ��������� ���� �� ������, ��������� � ���� ��� ������ new,
    // ���� �������� ������� ����������
    // ������������ noexcept ����������, ��� ����� �� ������� ����������
    explicit ScopedPtr(T* raw_ptr) noexcept : ptr_(raw_ptr) {
        // ������ ������ �� ����� - �������������� ������������� ptr_
    }

    // ������� � ������ ����������� �����������
    ScopedPtr(const ScopedPtr&) = delete;

    // ����������. ������� ������, �� ������� ��������� ����� ���������.
    ~ScopedPtr() {
        delete ptr_; // ������� ������, �� ������� ��������� ptr_
    }

    // ���������� ���������, ���������� ������ ScopedPtr
    T* GetRawPtr() const noexcept {
        return ptr_; // ������ ���������� �������� ptr_
    }

    // ���������� �������� ��������, �� ������� ��������� ����� ���������.
    // ���������� ������� �������� "������" ��������� � ������������� ���� ptr_ � null
    T* Release() noexcept {
        T* temp = ptr_; // ��������� ������� ���������
        ptr_ = nullptr; // ������������� ptr_ � nullptr, ����� ���������� ��������
        return temp; // ���������� ����������� ���������
    }

private:
    T* ptr_ = nullptr; // ��������� �� ������, ������� ������� ScopedPtr
};

// ���� main ��������� ����� ScopedPtr
int main() {
    // ��������������� "�����", ����������� ������ � ����� ��������
    struct DeletionSpy {
        explicit DeletionSpy(bool& is_deleted)
            : is_deleted_(is_deleted) {
        }
        ~DeletionSpy() {
            is_deleted_ = true;
        }
        bool& is_deleted_;
    };

    // ��������� �������������� ��������
    {
        bool is_deleted = false;
        {
            // ����������� "�����", ����� ��� ����� �������� �� �������� is_deleted � true
            DeletionSpy* raw_ptr = new DeletionSpy(is_deleted);
            ScopedPtr<DeletionSpy> p(raw_ptr);
            assert(p.GetRawPtr() == raw_ptr);
            assert(!is_deleted);
            // ��� ������ �� ����� ���������� p ������ ������� "������"
        }
        // ���� ���������� ������ ��������� �������� ���������, ����� ����� ����� "�������"
        // ������ ��������� is_deleted � true
        assert(is_deleted);
    }

    // ��������� ������ ������ Release
    {
        bool is_deleted = false;
        DeletionSpy* raw_ptr = new DeletionSpy(is_deleted);
        {
            ScopedPtr<DeletionSpy> scoped_ptr(raw_ptr);
            assert(scoped_ptr.Release() == raw_ptr);
            assert(scoped_ptr.GetRawPtr() == nullptr);
            // ����� Release ����� ��������� �� ��������� �� ������ � �� ������� ��� ��� ����� ��������
        }
        assert(!is_deleted);
        delete raw_ptr;
        assert(is_deleted);
    }
}