#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

namespace {
class Formula : public FormulaInterface {
public:
    explicit Formula(std::string expression) try 
        : ast_(ParseFormulaAST(expression)) {
    } catch (const FormulaException& e) {
        throw;
    } catch (const std::exception& e) {
        throw FormulaException("Formula parsing error: "s + e.what());
    }

    Value Evaluate() const override {
        try {
            return ast_.Execute();
        } catch (const FormulaError&) {
            return FormulaError("ARITHM");
        } catch (...) {
            return FormulaError("ARITHM");
        }
    }

    std::string GetExpression() const override {
        std::ostringstream out;
        ast_.PrintFormula(out);
        return out.str();
    }

private:
    FormulaAST ast_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    return std::make_unique<Formula>(std::move(expression));
}