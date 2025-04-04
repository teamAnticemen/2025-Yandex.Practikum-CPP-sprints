#include <cassert>
#include <stdexcept> // ��� std::logic_error

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

    // �������������� � ���� bool ��� �������� �� nullptr
    explicit operator bool() const noexcept {
        return ptr_ != nullptr; // ���������� true, ���� ptr_ �� ����� nullptr
    }

    // �������� �������������
    T& operator*() const {
        if (!ptr_) {
            throw std::logic_error("Dereferencing a null pointer");
        }
        return *ptr_; // ���������� �������������� ���������
    }

    // �������� ������� � ����� ������
    T* operator->() const {
        if (!ptr_) {
            throw std::logic_error("Accessing a member of a null pointer");
        }
        return ptr_; // ���������� ���������
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

    // ��������� ������ ���������� ������������� � ������� � ������
    {
        bool is_deleted = false;
        DeletionSpy* raw_ptr = new DeletionSpy(is_deleted);
        ScopedPtr<DeletionSpy> p(raw_ptr);
        assert(static_cast<bool>(p)); // ��������, ��� ��������� �� �������
        assert(p.GetRawPtr() == raw_ptr);
        
        // �������� �������������
        assert(&(*p) == raw_ptr);
        
        // �������� ������� � ����� ����� ��������->
        assert(p->is_deleted_ == false); // ��������, ��� ���� ������ �����
    }

    // �������� ������� ����������
    {
        ScopedPtr<DeletionSpy> p;
        try {
            *p; // ������ ��������� ����������
        } catch (const std::logic_error& e) {
            assert(std::string(e.what()) == "Dereferencing a null pointer");
        }
        
        try {
            p->is_deleted_; // ������ ��������� ����������
        } catch (const std::logic_error& e) {
            assert(std::string(e.what()) == "Accessing a member of a null pointer");
        }
    }

    return 0;
}