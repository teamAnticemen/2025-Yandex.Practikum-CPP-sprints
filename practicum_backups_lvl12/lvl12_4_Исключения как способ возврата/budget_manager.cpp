#include "budget_manager.h"
#include "date.h"

using namespace std::literals;

const Date BudgetManager::START_DATE = Date("2000-01-01"s);
const Date BudgetManager::END_DATE = Date("2100-01-01"s);

BudgetManager::BudgetManager() : days(365 * (2099 - 2000 + 1)) {}

int BudgetManager::DateToIndex(const Date& date) const {
    return Date::ComputeDistance(START_DATE, date);
}

void BudgetManager::Earn(const Date& start_date, const Date& end_date, double amount) {
    int days_count = Date::ComputeDistance(start_date, end_date) + 1;
    double daily_income = amount / days_count;

    int start_index = DateToIndex(start_date);
    int end_index = DateToIndex(end_date);

    for (int i = start_index; i <= end_index; ++i) {
        days[i].income += daily_income;
    }
}

void BudgetManager::Spend(const Date& start_date, const Date& end_date, double amount) {
    int days_count = Date::ComputeDistance(start_date, end_date) + 1;
    double daily_spent = amount / days_count;

    int start_index = DateToIndex(start_date);
    int end_index = DateToIndex(end_date);

    for (int i = start_index; i <= end_index; ++i) {
        days[i].spent += daily_spent;
    }
}

void BudgetManager::PayTax(const Date& start_date, const Date& end_date, int tax_rate) {
    double tax_multiplier = 1.0 - tax_rate / 100.0;

    int start_index = DateToIndex(start_date);
    int end_index = DateToIndex(end_date);

    for (int i = start_index; i <= end_index; ++i) {
        days[i].income *= tax_multiplier; // Налог применяется только к доходам
    }
}

double BudgetManager::ComputeIncome(const Date& start_date, const Date& end_date) const {
    double total_income = 0.0;
    int start_index = DateToIndex(start_date);
    int end_index = DateToIndex(end_date);

    for (int i = start_index; i <= end_index; ++i) {
        total_income += (days[i].income - days[i].spent); // Чистая прибыль за день
    }
    return total_income;
}