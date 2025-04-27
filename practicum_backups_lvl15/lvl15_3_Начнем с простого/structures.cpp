#include "common.h"

#include <cctype>
#include <sstream>

const int LETTERS = 26;
const int MAX_POSITION_LENGTH = 17;
const int MAX_POS_LETTER_COUNT = 3;

const Position Position::NONE = {-1, -1};

bool Position::operator==(const Position rhs) const {
    return row == rhs.row && col == rhs.col;
}

bool Position::operator<(const Position rhs) const {
    return std::tie(row, col) < std::tie(rhs.row, rhs.col);
}

bool Position::IsValid() const {
    return row >= 0 && col >= 0 && row < MAX_ROWS && col < MAX_COLS;
}

std::string Position::ToString() const {
    if (!IsValid()) {
        return "";
    }

    std::string result;
    int column = col;
    while (column >= 0) {
        result.insert(result.begin(), 'A' + (column % LETTERS));
        column = column / LETTERS - 1;
    }

    return result + std::to_string(row + 1);
}

Position Position::FromString(std::string_view str) {
    if (str.empty()) {
        return Position::NONE;
    }

    size_t i = 0;
    // Parse letters
    int col = 0;
    bool has_letters = false;
    while (i < str.size() && isalpha(str[i]) && isupper(str[i])) {
        if (i >= MAX_POS_LETTER_COUNT) {
            return Position::NONE;
        }
        col = col * LETTERS + (str[i] - 'A' + 1);
        ++i;
        has_letters = true;
    }
    if (!has_letters) {
        return Position::NONE;
    }
    col--;

    // Parse digits
    int row = 0;
    bool has_digits = false;
    while (i < str.size() && isdigit(str[i])) {
        row = row * 10 + (str[i] - '0');
        ++i;
        has_digits = true;
    }
    if (!has_digits || i != str.size()) {
        return Position::NONE;
    }
    row--;

    Position result{row, col};
    return result.IsValid() ? result : Position::NONE;
}