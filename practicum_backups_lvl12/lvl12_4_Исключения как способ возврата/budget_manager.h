#pragma once

#include "date.h"
#include <vector>
#include <string>

class BudgetManager {
public:
    static const Date START_DATE;
    static const Date END_DATE;

    BudgetManager();

    void Earn(const Date& start_date, const Date& end_date, double amount);
    void Spend(const Date& start_date, const Date& end_date, double amount);
    void PayTax(const Date& start_date, const Date& end_date, int tax_rate);
    double ComputeIncome(const Date& start_date, const Date& end_date) const;

private:
    struct DayInfo {
        double income = 0.0; // Заработанное
        double spent = 0.0;  // Потраченное
    };

    std::vector<DayInfo> days;

    int DateToIndex(const Date& date) const;
};