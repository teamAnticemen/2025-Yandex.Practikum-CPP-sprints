#include <cassert>
#include <cstdlib>
#include <utility> // ��� std::swap

template <typename Type>
class ArrayPtr {
public:
    // �������������� ArrayPtr ������� ����������
    ArrayPtr() = default;

    // ������� � ���� ������ �� size ��������� ���� Type.
    // ���� size == 0, ���� raw_ptr_ ������ ���� ����� nullptr
    explicit ArrayPtr(size_t size) {
        raw_ptr_ = size > 0 ? new Type[size] : nullptr;
    }

    // ����������� �� ������ ���������, ��������� ����� ������� � ���� ���� nullptr
    explicit ArrayPtr(Type* raw_ptr) noexcept : raw_ptr_(raw_ptr) {}

    // ��������� �����������
    ArrayPtr(const ArrayPtr&) = delete;

    ~ArrayPtr() {
        delete[] raw_ptr_;
    }

    // ��������� ������������
    ArrayPtr& operator=(const ArrayPtr&) = delete;

    // ���������� ��������� �������� � ������, ���������� �������� ������ �������
    // ����� ������ ������ ��������� �� ������ ������ ����������
    [[nodiscard]] Type* Release() noexcept {
        Type* temp = raw_ptr_;
        raw_ptr_ = nullptr;
        return temp;
    }

    // ���������� ������ �� ������� ������� � �������� index
    Type& operator[](size_t index) noexcept {
        return raw_ptr_[index];
    }

    // ���������� ����������� ������ �� ������� ������� � �������� index
    const Type& operator[](size_t index) const noexcept {
        return raw_ptr_[index];
    }

    // ���������� true, ���� ��������� ���������, � false � ��������� ������
    explicit operator bool() const {
        return raw_ptr_ != nullptr;
    }

    // ���������� �������� ������ ���������, ��������� ����� ������ �������
    Type* Get() const noexcept {
        return raw_ptr_;
    }

    // ������������ ��������� ��������� �� ������ � �������� other
    void swap(ArrayPtr& other) noexcept {
        std::swap(raw_ptr_, other.raw_ptr_);
    }

private:
    Type* raw_ptr_ = nullptr;
};

int main() {
    ArrayPtr<int> numbers(10);
    const auto& const_numbers = numbers;

    numbers[2] = 42;
    assert(const_numbers[2] == 42);
    assert(&const_numbers[2] == &numbers[2]);

    assert(numbers.Get() == &numbers[0]);

    ArrayPtr<int> numbers_2(5);
    numbers_2[2] = 43;

    numbers.swap(numbers_2);

    assert(numbers_2[2] == 42);
    assert(numbers[2] == 43);
}