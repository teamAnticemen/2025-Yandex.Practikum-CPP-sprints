#include "budget_manager.h"

using namespace std::literals;

const Date BudgetManager::START_DATE = Date("2000-01-01"s);
const Date BudgetManager::END_DATE = Date("2100-01-01"s);

BudgetManager::BudgetManager() : tree_(Date::ComputeDistance(START_DATE, END_DATE) + 1) {}

int BudgetManager::DateToIndex(const Date& date) const {
    return Date::ComputeDistance(START_DATE, date);
}

void BudgetManager::Earn(const Date& start_date, const Date& end_date, double amount) {
    int days_count = Date::ComputeDistance(start_date, end_date) + 1;
    double daily_income = amount / days_count;

    IndexSegment segment{static_cast<size_t>(DateToIndex(start_date)),
                        static_cast<size_t>(DateToIndex(end_date) + 1)};
    tree_.AddBulkOperation(segment, BulkMoneyAdder{daily_income});
}

void BudgetManager::Spend(const Date& start_date, const Date& end_date, double amount) {
    int days_count = Date::ComputeDistance(start_date, end_date) + 1;
    double daily_spent = amount / days_count;

    IndexSegment segment{static_cast<size_t>(DateToIndex(start_date)),
                        static_cast<size_t>(DateToIndex(end_date) + 1)};
    tree_.AddBulkOperation(segment, BulkMoneySpender{daily_spent});
}

void BudgetManager::PayTax(const Date& start_date, const Date& end_date, int tax_rate) {
    double tax_multiplier = 1.0 - tax_rate / 100.0;

    IndexSegment segment{static_cast<size_t>(DateToIndex(start_date)),
                        static_cast<size_t>(DateToIndex(end_date) + 1)};
    tree_.AddBulkOperation(segment, BulkTaxApplier{tax_multiplier});
}

double BudgetManager::ComputeIncome(const Date& start_date, const Date& end_date) const {
    IndexSegment segment{static_cast<size_t>(DateToIndex(start_date)),
                        static_cast<size_t>(DateToIndex(end_date) + 1)};
    DayBudget result = tree_.ComputeSum(segment);
    return result.income - result.spend;
}