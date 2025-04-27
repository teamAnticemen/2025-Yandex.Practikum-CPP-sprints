#include "sheet.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}

void Sheet::ValidatePosition(Position pos) const {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid position");
    }
}

void Sheet::UpdatePrintableSize() {
    int max_row = -1;
    int max_col = -1;

    for (const auto& [pos, _] : cells_) {
        max_row = std::max(max_row, pos.row);
        max_col = std::max(max_col, pos.col);
    }

    printable_size_.rows = max_row >= 0 ? max_row + 1 : 0;
    printable_size_.cols = max_col >= 0 ? max_col + 1 : 0;
}

void Sheet::SetCell(Position pos, std::string text) {
    ValidatePosition(pos);

    auto& cell = cells_[pos];
    if (!cell) {
        cell = std::make_unique<Cell>();
    }
    cell->Set(std::move(text));

    UpdatePrintableSize();
}

const CellInterface* Sheet::GetCell(Position pos) const {
    ValidatePosition(pos);

    auto it = cells_.find(pos);
    return it != cells_.end() ? it->second.get() : nullptr;
}

CellInterface* Sheet::GetCell(Position pos) {
    return const_cast<CellInterface*>(
        static_cast<const Sheet*>(this)->GetCell(pos)
    );
}

void Sheet::ClearCell(Position pos) {
    ValidatePosition(pos);

    if (cells_.erase(pos)) {
        UpdatePrintableSize();
    }
}

Size Sheet::GetPrintableSize() const {
    return printable_size_;
}

void Sheet::PrintValues(std::ostream& output) const {
    for (int row = 0; row < printable_size_.rows; ++row) {
        for (int col = 0; col < printable_size_.cols; ++col) {
            if (col > 0) {
                output << '\t';
            }
            if (auto it = cells_.find({row, col}); it != cells_.end()) {
                std::visit([&](const auto& x) { output << x; }, it->second->GetValue());
            }
        }
        output << '\n';
    }
}

void Sheet::PrintTexts(std::ostream& output) const {
    for (int row = 0; row < printable_size_.rows; ++row) {
        for (int col = 0; col < printable_size_.cols; ++col) {
            if (col > 0) {
                output << '\t';
            }
            if (auto it = cells_.find({row, col}); it != cells_.end()) {
                output << it->second->GetText();
            }
        }
        output << '\n';
    }
}