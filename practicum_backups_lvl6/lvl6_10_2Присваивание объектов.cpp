#include <iostream>
#include <vector>
#include <cassert>
#include <stdexcept>
#include <utility>
#include <string>

using namespace std;

// Реализация класса ScopedPtr
template <typename T>
class ScopedPtr {
public:
    ScopedPtr() = default;

    explicit ScopedPtr(T* raw_ptr) noexcept : ptr_(raw_ptr) {}

    ScopedPtr(ScopedPtr&& other) noexcept : ptr_(other.Release()) {}

    ScopedPtr& operator=(ScopedPtr&& other) noexcept {
        if (this == &other) return *this;
        delete ptr_;
        ptr_ = other.Release();
        return *this;
    }

    ScopedPtr(const ScopedPtr&) = delete; // Запрещаем копирование указателя

    ~ScopedPtr() { delete ptr_; }

    T* GetRawPtr() const noexcept { return ptr_; }

    T* Release() noexcept {
        T* p = ptr_;
        ptr_ = nullptr;
        return p;
    }

    explicit operator bool() const { return ptr_ != nullptr; }

    T* operator->() const {
        if (!ptr_) {
            throw std::logic_error("Scoped ptr is null");
        }
        return ptr_;
    }

    T& operator*() const {
        if (!ptr_) {
            throw std::logic_error("Scoped ptr is null");
        }
        return *ptr_;
    }

private:
    T* ptr_ = nullptr;
};

// Реализация класса PtrVector
template <typename T>
class PtrVector {
public:
    PtrVector() = default;

    PtrVector(const PtrVector& other) {
        items_.reserve(other.items_.size());
        for (const auto& p : other.items_) {
            ScopedPtr<T> p_copy(p ? new T(*p) : nullptr);
            items_.emplace_back(std::move(p_copy));
        }
    }

    PtrVector& operator=(const PtrVector& other) {
        if (this == &other) return *this;
        DeleteItems();
        items_.reserve(other.items_.size());
        for (const auto& p : other.items_) {
            ScopedPtr<T> p_copy(p ? new T(*p) : nullptr);
            items_.emplace_back(std::move(p_copy));
        }
        return *this;
    }

    ~PtrVector() { DeleteItems(); }

    void Add(T* ptr) {
        items_.emplace_back(ptr);
    }

    std::vector<ScopedPtr<T>>& GetItems() noexcept {
        return items_;
    }

    const std::vector<ScopedPtr<T>>& GetItems() const noexcept {
        return items_;
    }

private:
    void DeleteItems() noexcept {
        items_.clear(); // ScopedPtr автоматически управляет памятью
    }

    std::vector<ScopedPtr<T>> items_;
};

// Реализация класса Tentacle
class Tentacle {
public:
    explicit Tentacle(int id) noexcept : id_(id), linked_tentacle_(nullptr) {}

    int GetId() const noexcept { return id_; }

    void LinkTo(Tentacle& other) { linked_tentacle_ = &other; }

    void Unlink() { linked_tentacle_ = nullptr; } // Метод Unlink

    Tentacle* GetLinkedTentacle() const noexcept { return linked_tentacle_; }

private:
    int id_;
    Tentacle* linked_tentacle_;
};

class Octopus {
public:
    Octopus() : Octopus(8) {}

    explicit Octopus(int num_tentacles) {
        for (int i = 0; i < num_tentacles; i++) {
            tentacles_.Add(new Tentacle(i));
        }
    }

    Octopus(const Octopus& other) {
        for (const auto& p : other.tentacles_.GetItems()) {
            tentacles_.Add(new Tentacle(*p)); // создаем копию каждого щупальца
        }
    }

    int GetTentacleCount() const noexcept { return static_cast<int>(tentacles_.GetItems().size()); }

    Tentacle& GetTentacle(size_t index) { return *tentacles_.GetItems()[index]; }

    const Tentacle& GetTentacle(size_t index) const { return *tentacles_.GetItems()[index]; }

    // Добавление нового щупальца
    Tentacle& AddTentacle() {
        int new_id = GetTentacleCount() + 1; // Определяем новый ID
        Tentacle* new_tentacle = new Tentacle(new_id); // Создаем новое щупальце с этим ID
        tentacles_.Add(new_tentacle); // Добавляем его в PtrVector
        return *new_tentacle; // Возвращаем ссылку на новое щупальце
    }

    Octopus& operator=(const Octopus& other) {
        if (this == &other) return *this;
        tentacles_ = other.tentacles_;
        return *this;
    }

private:
    PtrVector<Tentacle> tentacles_; // Используем PtrVector для хранения щупалец
};

// Тесты осьминога и щупалец
int main() {
    // Проверка присваивания осьминогов
    {
        Octopus octopus1(3);
        octopus1.GetTentacle(2).LinkTo(octopus1.GetTentacle(1));

        Octopus octopus2(10);
        octopus2 = octopus1;

        assert(octopus2.GetTentacleCount() == octopus1.GetTentacleCount());
        for (int i = 0; i < octopus2.GetTentacleCount(); ++i) {
            auto& tentacle1 = octopus1.GetTentacle(i);
            auto& tentacle2 = octopus2.GetTentacle(i);
            assert(&tentacle2 != &tentacle1);
            assert(tentacle2.GetId() == tentacle1.GetId());
            assert(tentacle2.GetLinkedTentacle() == tentacle1.GetLinkedTentacle());
        }
    }

    // Проверка самоприсваивания осьминогов
    {
        Octopus octopus(3);
        octopus.GetTentacle(0).LinkTo(octopus.GetTentacle(1));

        std::vector<std::pair<Tentacle*, Tentacle*>> tentacles;
        for (int i = 0; i < octopus.GetTentacleCount(); ++i) {
            tentacles.push_back({&octopus.GetTentacle(i), octopus.GetTentacle(i).GetLinkedTentacle()});
        }

        octopus = octopus;

        assert(octopus.GetTentacleCount() == static_cast<int>(tentacles.size()));
        for (int i = 0; i < octopus.GetTentacleCount(); ++i) {
            auto& tentacle_with_link = tentacles.at(i);
            assert(&octopus.GetTentacle(i) == tentacle_with_link.first);
            assert(octopus.GetTentacle(i).GetLinkedTentacle() == tentacle_with_link.second);
        }
    }

    // Проверка метода AddTentacle
    {
        Octopus o;
        Tentacle* t = &o.AddTentacle(); // Правильный способ без использования &
        assert(t != nullptr); // Утверждаем, что указатель не нулевой
        assert(o.GetTentacleCount() == 2); // Ожидается 2 щупальца
    }

    std::cout << "All tests passed successfully!" << std::endl;
    return 0;
}