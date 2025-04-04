#include "libstat.h"
#include <algorithm>
#include <cmath>

namespace statistics {
namespace aggregations {

void Sum::PutValue(double value) {
    sum_ += value; // Суммируем значение
}

    std::optional<double> Sum::Get() const {
    return sum_; // Возвращаем сумму, даже если она равна нулю
}

void Max::PutValue(double value) {
    if (!cur_max_ || value > *cur_max_) {
        cur_max_ = value; // Обновляем текущее максимальное значение
    }
}

    std::optional<double> Max::Get() const {
    return cur_max_; // Возвращаем текущее максимальное значение
}

void Mean::PutValue(double value) {
    sum_.PutValue(value);
    ++count_;
}

std::optional<double> Mean::Get() const {
    auto val = sum_.Get();
    if (!val || count_ == 0) {
        return std::nullopt;
    }
    return *val / count_;
}

void StandardDeviation::PutValue(double value) {
    sum_.PutValue(value);
    sum_sq_.PutValue(value * value);
    ++count_;
}

std::optional<double> StandardDeviation::Get() const {
    auto val = sum_.Get();
    auto val2 = sum_sq_.Get();

    if (!val || !val2 || count_ < 2) {
        return std::nullopt;
    }

    return std::sqrt((*val2 - *val * *val / count_) / count_);
}

void Mode::PutValue(double value) {
    const size_t new_count = ++counts_[std::round(value)];

    if (new_count > cur_count_) {
        cur_max_ = value;
        cur_count_ = new_count;
    }
}

    std::optional<double> Mode::Get() const {
    return cur_max_;
}

} // namespace aggregations
} // namespace statistics
