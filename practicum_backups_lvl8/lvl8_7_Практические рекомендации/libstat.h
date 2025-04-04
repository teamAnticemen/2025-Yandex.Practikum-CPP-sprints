#pragma once
#include <algorithm>
#include <cmath>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <sstream>

using namespace std::literals::string_view_literals; // Для использования "sv"

namespace statistics {
    namespace aggregations {
        class Sum {
        public:
            void PutValue(double value);
            std::optional<double> Get() const;

            static std::string_view GetValueName() {
                return "sum"sv;
            }

        private:
            double sum_ = 0; // Начальное значение суммы
        };

        class Max {
        public:
            void PutValue(double value);
            std::optional<double> Get() const;

            static std::string_view GetValueName() {
                return "max";
            }
        private:
            std::optional<double> cur_max_;
        };

        class Mean {
        public:
            void PutValue(double value);
            std::optional<double> Get() const;

            static std::string_view GetValueName() {
                return "mean";
            }
        private:
            Sum sum_;
            size_t count_ = 0;
        };

        class StandardDeviation {
        public:
            void PutValue(double value);
            std::optional<double> Get() const;

            static std::string_view GetValueName() {
                return "standard deviation";
            }
        private:
            Sum sum_;
            Sum sum_sq_;
            size_t count_ = 0;
        };

        class Mode {
        public:
            void PutValue(double value);
            std::optional<double> Get() const;

            static std::string_view GetValueName() {
                return "mode";
            }
        private:
            std::unordered_map<double, size_t> counts_;
            std::optional<double> cur_max_;
            size_t cur_count_ = 0;
        };
    } // namespace aggregations

    namespace detail {
        // Перемещение AggregPrinter в пространство имен detail
        template <typename T>
        class AggregPrinter {
        public:
            void PutValue(double value) {
                inner_.PutValue(value);
            }

            void Print(std::ostream& out) const {
                auto val = inner_.Get();
                out << inner_.GetValueName() << " is ";
                if (val) {
                    out << *val;
                } else {
                    out << "undefined";
                }
                out << std::endl;
            }

        private:
            T inner_;
        };

        template <typename T>
        std::string GetPrinterValue(AggregPrinter<T>& printer) {
            std::ostringstream out; 
            printer.Print(out);
            return out.str();
        }
    } // namespace detail

    // Обертка для использования AggregPrinter и GetPrinterValue в пространстве имен statistics
    template <typename T>
    using AggregPrinter = detail::AggregPrinter<T>;

    template <typename T>
    std::string GetPrinterValue(AggregPrinter<T>& printer) {
        return detail::GetPrinterValue(printer);
    }

    namespace tests {
        void AggregSum();
        void AggregMax();
        void AggregMean();
        void AggregStandardDeviation();
        void AggregMode();
        void AggregPrinter();
    } // namespace tests
} // namespace statistics