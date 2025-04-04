#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

// не меняйте файлы xml.h и xml.cpp
#include "xml.h"

using namespace std;

struct Spending {
    string category;
    int amount;
};

int CalculateTotalSpendings(const vector<Spending>& spendings) {
    return accumulate(spendings.begin(), spendings.end(), 0,
                      [](int current, const Spending& spending) {
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

vector<Spending> LoadFromXml(istream& input) {
    vector<Spending> spendings;

    // Загружаем XML-документ
    Document doc = Load(input);
    
    // Получаем корневой узел документа
    const Node& root = doc.GetRoot();
    
    // Итерируем по дочерним элементам корневого узла
    for (const Node& child : root.Children()) {
        // Проверяем, что это тег spend
        if (child.Name() == "spend") {
            // Извлекаем атрибуты amount и category
            int amount = child.AttributeValue<int>("amount");
            string category = child.AttributeValue<string>("category");
            
            // Добавляем вектор трат
            spendings.push_back({move(category), amount});
        }
    }
    
    return spendings;
}


int main() {
    const vector<Spending> spendings = LoadFromXml(cin);
    cout << "Total "sv << CalculateTotalSpendings(spendings) << '\n';
    cout << "Most expensive is "sv << FindMostExpensiveCategory(spendings) << '\n';
}