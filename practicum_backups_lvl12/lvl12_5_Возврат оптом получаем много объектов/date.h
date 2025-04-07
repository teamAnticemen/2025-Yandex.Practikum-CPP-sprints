#pragma once

#include <chrono>
#include <sstream>
#include <string>
#include <string_view>

class Date {
public:
    explicit Date(const std::string& str) {
        std::istringstream input(str);
        std::chrono::from_stream(input, "%Y-%m-%d", ymd_);
    }

    explicit Date(std::string_view str)
        : Date(std::string(str)) {
    }

    static int ComputeDistance(const Date& from, const Date& to) {
        return static_cast<int>(
            (std::chrono::sys_days(to.ymd_) - std::chrono::sys_days(from.ymd_)).count());
    }

    int GetYear() const {
        return static_cast<int>(ymd_.year());
    }

    int GetMonth() const {
        return static_cast<unsigned>(ymd_.month());
    }

    int GetDay() const {
        return static_cast<unsigned>(ymd_.day());
    }

private:
    std::chrono::year_month_day ymd_{};
};