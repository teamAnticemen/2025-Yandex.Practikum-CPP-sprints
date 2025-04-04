#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Щупальце
class Tentacle {
public:
    explicit Tentacle(int id) : id_(id) {}

    int GetId() const {
        return id_;
    }

private:
    int id_ = 0;
};

// Осьминог
class Octopus {
public:
    Octopus() {
        Tentacle* t = nullptr;
        try {
            for (int i = 1; i <= 8; ++i) {
                t = new Tentacle(i);
                tentacles_.push_back(t);
                t = nullptr;  // Не забываем обнулить указатель
            }
        } catch (const bad_alloc&) {
            Cleanup();
            delete t; // Удаляем указатель t, если он установлен
            throw;
        }
    }

    // Конструктор копирования
    Octopus(const Octopus& other) {
        for (const Tentacle* t : other.tentacles_) {
            tentacles_.push_back(new Tentacle(*t)); // Копируем щупальца
        }
    }

    // Оператор присваивания
    Octopus& operator=(const Octopus& other) {
        if (this == &other) {
            return *this; // Защита от самоприсваивания
        }

        Cleanup(); // Удаляем старые щупальца

        for (const Tentacle* t : other.tentacles_) {
            tentacles_.push_back(new Tentacle(*t)); // Копируем щупальца
        }

        return *this;
    }

    const Tentacle& GetTentacle(int index) const {
        if (index < 0 || static_cast<size_t>(index) >= tentacles_.size()) {
            throw out_of_range("Invalid tentacle index"s);
        }
        return *tentacles_[index];
    }

    ~Octopus() {
        Cleanup();
    }

private:
    void Cleanup() {
        for (Tentacle* t : tentacles_) {
            delete t;
        }
        tentacles_.clear();
    }

    vector<Tentacle*> tentacles_;
};

int main() {
    // Создаем осьминога
    Octopus octopus1;

    // Создаем копию осьминога
    Octopus octopus2 = octopus1; // Здесь сработает конструктор копирования

    // Используем осьминога и его копию
    for (int i = 0; i < 8; ++i) {
        cout << "octopus1 tentacle " << octopus1.GetTentacle(i).GetId() << endl;
        cout << "octopus2 tentacle " << octopus2.GetTentacle(i).GetId() << endl;
    }

    // Создаем еще одну копию через оператор присваивания
    Octopus octopus3;
    octopus3 = octopus1; // Здесь сработает оператор присваивания

    // Используем новую копию осьминога
    for (int i = 0; i < 8; ++i) {
        cout << "octopus3 tentacle " << octopus3.GetTentacle(i).GetId() << endl;
    }

    return 0;
}