#include "cell.h"
#include <stdexcept>

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << fe.what();
}

Cell::Cell() : impl_(std::make_unique<EmptyImpl>()) {}

Cell::~Cell() = default;

void Cell::Set(std::string text) {
    std::unique_ptr<Impl> new_impl;
    
    if (text.empty()) {
        new_impl = std::make_unique<EmptyImpl>();
    }
    else if (text.size() > 1 && text[0] == FORMULA_SIGN) {
        try {
            new_impl = std::make_unique<FormulaImpl>(std::move(text));
        } catch (const FormulaException&) {
            throw; // Пробрасываем исключение, не меняя impl_
        } catch (...) {
            throw FormulaException("#ARITHM!");
        }
    }
    else {
        new_impl = std::make_unique<TextImpl>(std::move(text));
    }

    impl_ = std::move(new_impl);
}

void Cell::Clear() {
    impl_ = std::make_unique<EmptyImpl>();
}

Cell::Value Cell::GetValue() const {
    return impl_->GetValue();
}

std::string Cell::GetText() const {
    return impl_->GetText();
}