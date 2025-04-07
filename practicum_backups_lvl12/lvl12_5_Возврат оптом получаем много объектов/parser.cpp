#include "parser.h"
#include "date.h"
#include <sstream>
#include <iomanip>
#include <iostream>

void ParseAndProcessQuery(BudgetManager& manager, std::string_view line) {
    std::istringstream stream{std::string(line)};
    std::string command;
    stream >> command;

    if (command == "Earn") {
        std::string start_date_str, end_date_str;
        double amount;
        stream >> start_date_str >> end_date_str >> amount;
        Date start_date(start_date_str);
        Date end_date(end_date_str);
        manager.Earn(start_date, end_date, amount);
    } else if (command == "Spend") {
        std::string start_date_str, end_date_str;
        double amount;
        stream >> start_date_str >> end_date_str >> amount;
        Date start_date(start_date_str);
        Date end_date(end_date_str);
        manager.Spend(start_date, end_date, amount);
    } else if (command == "PayTax") {
        std::string start_date_str, end_date_str;
        int tax_rate;
        stream >> start_date_str >> end_date_str >> tax_rate;
        Date start_date(start_date_str);
        Date end_date(end_date_str);
        manager.PayTax(start_date, end_date, tax_rate);
    } else if (command == "ComputeIncome") {
        std::string start_date_str, end_date_str;
        stream >> start_date_str >> end_date_str;
        Date start_date(start_date_str);
        Date end_date(end_date_str);
        double income = manager.ComputeIncome(start_date, end_date);
        std::cout << std::setprecision(6) << income << std::endl;
    }
}