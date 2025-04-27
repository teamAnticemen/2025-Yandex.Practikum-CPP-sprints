#pragma once

#include "common.h"
#include "formula.h"
#include <memory>
#include <string>

class Cell : public CellInterface {
public:
    Cell();
    ~Cell();

    void Set(std::string text) override;
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;

private:
    class Impl {
    public:
        virtual ~Impl() = default;
        virtual Value GetValue() const = 0;
        virtual std::string GetText() const = 0;
    };

    class EmptyImpl : public Impl {
    public:
        Value GetValue() const override { return ""; }
        std::string GetText() const override { return ""; }
    };

    class TextImpl : public Impl {
    public:
        explicit TextImpl(std::string text) : text_(std::move(text)) {}
        
        Value GetValue() const override {
            if (text_.empty()) return "";
            if (text_[0] == ESCAPE_SIGN) return text_.substr(1);
            return text_;
        }
        
        std::string GetText() const override { return text_; }

    private:
        std::string text_;
    };

    class FormulaImpl : public Impl {
    public:
        explicit FormulaImpl(std::string expression)
            : formula_(ParseFormula(expression.substr(1))),
              expression_("=" + formula_->GetExpression()) {}

        Value GetValue() const override {
            auto result = formula_->Evaluate();
            if (std::holds_alternative<double>(result)) {
                return std::get<double>(result);
            }
            return std::get<FormulaError>(result);
        }

        std::string GetText() const override {
            return expression_;
        }

    private:
        std::unique_ptr<FormulaInterface> formula_;
        std::string expression_;
    };

    std::unique_ptr<Impl> impl_;
};