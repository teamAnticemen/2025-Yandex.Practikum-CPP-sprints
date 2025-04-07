#pragma once

#include "date.h"
#include "summing_segment_tree.h"
#include "entities.h"
#include "bulk_updater.h"

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
    SummingSegmentTree<DayBudget, BulkLinearUpdater> tree_;

    int DateToIndex(const Date& date) const;
};