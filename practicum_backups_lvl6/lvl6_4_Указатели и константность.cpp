#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <tuple>

using namespace std;

// Породы кошек
enum class CatBreed {
    Bengal,
    Balinese,
    Persian,
    Siamese,
    Siberian,
    Sphynx,
};

// Пол
enum class Gender {
    Male,
    Female,
};

struct Cat {
    string name;
    Gender gender;
    CatBreed breed;
    int age;
};

string CatBreedToString(CatBreed breed) {
    switch (breed) {
        case CatBreed::Bengal:
            return "Bengal"s;
        case CatBreed::Balinese:
            return "Balinese"s;
        case CatBreed::Persian:
            return "Persian"s;
        case CatBreed::Siamese:
            return "Siamese"s;
        case CatBreed::Siberian:
            return "Siberian"s;
        case CatBreed::Sphynx:
            return "Sphynx"s;
        default:
            throw invalid_argument("Invalid cat breed");
    }
}

ostream& operator<<(ostream& out, CatBreed breed) {
    out << CatBreedToString(breed);
    return out;
}

ostream& operator<<(ostream& out, Gender gender) {
    out << (gender == Gender::Male ? "male" : "female");
    return out;
}

ostream& operator<<(ostream& out, const Cat& cat) {
    out << '{' << cat.name << ", " << cat.gender << ", breed: " << cat.breed << ", age:" << cat.age << '}';
    return out;
}

// Возвращает массив указателей на элементы вектора cats, отсортированные с использованием
// компаратора comp. Компаратор comp - функция, принимающая два аргумента типа const Cat&
// и возвращающая true, если значения упорядочены, и false в ином случае
template <typename Comparator>
vector<const Cat*> GetSortedCats(const vector<Cat>& cats, const Comparator& comp) {
    vector<const Cat*> sorted_cat_pointers;

    // Помещаем адреса объектов из массива cats в sorted_cat_pointers
    for (const auto& cat : cats) {
        sorted_cat_pointers.push_back(&cat);
    }

    // Сортируем sorted_cat_pointers с использованием компаратора
    sort(sorted_cat_pointers.begin(), sorted_cat_pointers.end(),
         [&comp](const Cat* lhs, const Cat* rhs) {
             return comp(*lhs, *rhs);
         });

    return sorted_cat_pointers;
}

// Выводит в поток out значения объектов, на которые ссылаются указатели вектора cat_pointers.
void PrintCatPointerValues(const vector<const Cat*>& cat_pointers, ostream& out) {
    out << '{';
    for (size_t i = 0; i < cat_pointers.size(); ++i) {
        out << *cat_pointers[i];
        if (i < cat_pointers.size() - 1) {
            out << ", ";
        }
    }
    out << '}';
}

int main() {
    const vector<Cat> cats = {
        {"Tom", Gender::Male, CatBreed::Bengal, 2},
        {"Leo", Gender::Male, CatBreed::Siberian, 3},
        {"Luna", Gender::Female, CatBreed::Siamese, 1},
        {"Charlie", Gender::Male, CatBreed::Balinese, 7},
        {"Ginger", Gender::Female, CatBreed::Sphynx, 5},
        {"Tom", Gender::Male, CatBreed::Siamese, 2},
    };

    {
        auto sorted_cats = GetSortedCats(cats, [](const Cat& lhs, const Cat& rhs) {
            return tie(lhs.breed, lhs.name) < tie(rhs.breed, rhs.name);
        });

        cout << "Cats sorted by breed and name:" << endl;
        PrintCatPointerValues(sorted_cats, cout);
        cout << endl;
    }

    {
        auto sorted_cats = GetSortedCats(cats, [](const Cat& lhs, const Cat& rhs) {
            return tie(lhs.gender, lhs.breed) < tie(rhs.gender, rhs.breed);
        });

        cout << "Cats sorted by gender and breed:" << endl;
        PrintCatPointerValues(sorted_cats, cout);
        cout << endl;
    }
    return 0;
}