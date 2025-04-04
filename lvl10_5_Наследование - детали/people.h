#pragma once

#include <string>
#include <memory>

class Person;

// Наблюдатель за состоянием человека.
class PersonObserver {
public:
    virtual void OnSatisfactionChanged(Person& person, int old_value, int new_value) {
        // Используем параметры для избежания предупреждений
        (void)person; // Приводим к типу void, чтобы подавить предупреждение
        (void)old_value;
        (void)new_value;
    }

protected:
    virtual ~PersonObserver() = default;
};

/*
    Человек.
    При изменении уровня удовлетворённости уведомляет
    связанного с ним наблюдателя
*/
class Person {
public:
    Person(const std::string& name, int age) 
        : name_(name), age_(age) {}

    virtual ~Person() = default;

    int GetSatisfaction() const {
        return satisfaction_;
    }

    const std::string& GetName() const {
        return name_;
    }

    void SetObserver(PersonObserver* observer) {
        observer_ = observer;
    }

    int GetAge() const {
        return age_;
    }

    virtual void Dance() { // Объявляем метод как виртуальный
        int old_satisfaction = satisfaction_;
        satisfaction_ += 1; // Увеличиваем уровень удовлетворённости
        NotifyObserver(old_satisfaction);
        dance_count_++;
    }

    int GetDanceCount() const {
        return dance_count_;
    }

    virtual void LiveADay() {
        // Подклассы могут переопределить этот метод
    }

protected:
    void NotifyObserver(int old_satisfaction) {
        if (observer_) {
            observer_->OnSatisfactionChanged(*this, old_satisfaction, satisfaction_);
        }
    }

    int satisfaction_ = 100; // Теперь это защищённый член
    int dance_count_ = 0; // Теперь это защищённый член

private:
    std::string name_;
    PersonObserver* observer_ = nullptr;
    int age_;
};

// Рабочий.
// День рабочего проходит за работой
class Worker : public Person {
public:
    Worker(const std::string& name, int age) 
        : Person(name, age) {}

    void Work() {
        int old_satisfaction = GetSatisfaction();
        satisfaction_ -= 5; // Теперь доступно, так как это защищённый член
        NotifyObserver(old_satisfaction);
        work_done_++;
    }

    void LiveADay() override {
        Work();
    }

    void Dance() override { // Теперь переопределяем метод
        int old_satisfaction = GetSatisfaction();
        if (GetAge() > 30 && GetAge() < 40) {
            satisfaction_ += 2; // Увеличение на 2 для рабочих 30-40 лет
        } else {
            satisfaction_ += 1; // Увеличение на 1 для остальных
        }
        NotifyObserver(old_satisfaction);
        dance_count_++;
    }

    int GetWorkDone() const {
        return work_done_;
    }

private:
    int work_done_ = 0;
};

// Студент.
// День студента проходит за учёбой
class Student : public Person {
public:
    Student(const std::string& name, int age) 
        : Person(name, age) {}

    void Study() {
        int old_satisfaction = GetSatisfaction();
        satisfaction_ -= 3; // Уменьшение на 3
        NotifyObserver(old_satisfaction);
        knowledge_level_++;
    }

    void LiveADay() override {
        Study();
    }

    int GetKnowledgeLevel() const {
        return knowledge_level_;
    }


private:
    int knowledge_level_ = 0;
};
