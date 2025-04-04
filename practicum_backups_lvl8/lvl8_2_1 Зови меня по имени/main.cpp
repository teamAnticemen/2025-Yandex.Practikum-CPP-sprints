#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

// не меняйте файлы json.h и json.cpp
#include "json.h"

using namespace std;

struct Spending {
    string category;
    int amount;
};

int CalculateTotalSpendings(const vector<Spending>& spendings) {
    return accumulate(
        spendings.begin(), spendings.end(), 0, [](int current, const Spending& spending){
            return current + spending.amount;
        });
}

string FindMostExpensiveCategory(const vector<Spending>& spendings) {
    assert(!spendings.empty());
    auto compare_by_amount = [](const Spending& lhs, const Spending& rhs) {
        return lhs.amount < rhs.amount;
    };
    return max_element(begin(spendings), end(spendings), compare_by_amount)->category;
}

vector<Spending> LoadFromJson(istream& input) {
    vector<Spending> spendings;
    
    // Загружаем JSON-документ
    Document doc = Load(input);
    
    // Получаем корневой узел
    const Node& root = doc.GetRoot();
    
    // Проверяем, что корневой узел является массивом
    const auto& array = root.AsArray();
    
    // Проходим по каждому элементу массива
    for (const Node& item : array) {
        // Получаем ассоциативный контейнер (словарь)
        const auto& item_map = item.AsMap();
        
        // Извлекаем значения "amount" и "category"
        int amount = item_map.at("amount").AsInt();
        string category = item_map.at("category").AsString();
        
        // Добавляем вектор трат
        spendings.push_back({move(category), amount});
    }
    
    return spendings;
}

int main() {
    // не меняйте main
    const vector<Spending> spendings = LoadFromJson(cin);
    cout << "Total "sv << CalculateTotalSpendings(spendings) << '\n';
    cout << "Most expensive is "sv << FindMostExpensiveCategory(spendings) << '\n';
}