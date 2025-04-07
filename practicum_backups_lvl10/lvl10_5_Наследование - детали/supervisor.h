#pragma once

#include "people.h"

/*
    Надзиратель за уровнем удовлетворённости.
    Способен наблюдать за состоянием человека.
    Если уровень удовлетворённости человека опустится ниже минимального уровня, 
    Надзиратель побуждает человека танцевать до тех пор, пока уровень удовлетворённости
    не станет больше или равен максимальному значению.
*/
class SatisfactionSupervisor : public PersonObserver {
public:
    SatisfactionSupervisor(int min_satisfaction, int max_satisfaction)
        : min_satisfaction_(min_satisfaction), max_satisfaction_(max_satisfaction) {}

void OnSatisfactionChanged(Person& person, int old_value, int new_value) override {
    // Используем параметры для избежания предупреждений
    (void)old_value; // Приводим к типу void, чтобы подавить предупреждение

    // Проверяем, если уровень удовлетворённости ниже минимального
    if (new_value < min_satisfaction_ && new_value <= old_value) {
        // Если уровень удовлетворённости упал ниже минимального, заставляем танцевать
        while (new_value < max_satisfaction_) {
            person.Dance();  // Заставляем человека танцевать
            new_value = person.GetSatisfaction();  // Обновляем уровень удовлетворённости
        }
    }
}

private:
    int min_satisfaction_;  // Минимальный уровень удовлетворённости
    int max_satisfaction_;  // Максимальный уровень удовлетворённости
};
