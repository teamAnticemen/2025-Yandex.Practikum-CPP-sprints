#include "cell.h"
#include "common.h"
#include "formula.h"
#include "test_runner_p.h"

inline std::ostream& operator<<(std::ostream& output, const CellInterface::Value& value) {
    std::visit([&](const auto& x) { output << x; }, value);
    return output;
}

std::unique_ptr<CellInterface> CreateCell(const std::string& str) {
    std::unique_ptr<CellInterface> cell = std::make_unique<Cell>();
    cell->Set(str);
    return cell;
}

template<class T = double>
    void TestEvaluate(const std::string& expression) {
        std::cout << std::get<T>(ParseFormula(expression)->Evaluate()) << std::endl;
    }

int main()
{
        TestEvaluate<FormulaError>("1/0");
        TestEvaluate<FormulaError>("0/0");

        TestEvaluate<FormulaError>("1/(1-1)");
        TestEvaluate<FormulaError>("0/(1-1)");
        TestEvaluate<FormulaError>("(1-1)/(1-1)");

        TestEvaluate<FormulaError>("1+1/(1-1)");
        TestEvaluate<FormulaError>("1+0/(1-1)");
        TestEvaluate<FormulaError>("1+(1-1)/(1-1)");

        TestEvaluate<FormulaError>("1/(1-1)+1");
        TestEvaluate<FormulaError>("0/(1-1)+1");
        TestEvaluate<FormulaError>("(1-1)/(1-1)+1");
}