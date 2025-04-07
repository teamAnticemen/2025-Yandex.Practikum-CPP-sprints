#pragma once
#include <string>
#include <unordered_set>

enum class ProgrammingLanguage { CPP, JAVA, PYTHON, PHP };
enum class Gender { MALE, FEMALE };

class Person {
protected:
    std::string name;
    int age;
    Gender gender;

public:
    Person(const std::string& name, int age, Gender gender)
        : name(name), age(age), gender(gender) {}

    const std::string& GetName() const { return name; }
    int GetAge() const { return age; }
    Gender GetGender() const { return gender; }
};

// Программист. Знает несколько языков программирования
class Programmer : public Person {
private:
    std::unordered_set<ProgrammingLanguage> languages;

public:
    Programmer(const std::string& name, int age, Gender gender)
        : Person(name, age, gender) {}

    void AddProgrammingLanguage(ProgrammingLanguage language) {
        languages.insert(language);
    }

    bool CanProgram(ProgrammingLanguage language) const {
        return languages.find(language) != languages.end();
    }
};

// Специальности рабочего
enum class WorkerSpeciality { BLACKSMITH, CARPENTER, WOOD_CHOPPER, ENGINEER, PLUMBER };

// Рабочий. Владеет несколькими специальностями
class Worker : public Person {
private:
    std::unordered_set<WorkerSpeciality> specialities;

public:
    Worker(const std::string& name, int age, Gender gender)
        : Person(name, age, gender) {}

    void AddSpeciality(WorkerSpeciality speciality) {
        specialities.insert(speciality);
    }

    bool HasSpeciality(WorkerSpeciality speciality) const {
        return specialities.find(speciality) != specialities.end();
    }
};
