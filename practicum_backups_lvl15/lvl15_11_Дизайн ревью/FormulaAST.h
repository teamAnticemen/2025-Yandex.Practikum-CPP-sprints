#pragma once

#include "common.h"
#include "formula.h"

#include <forward_list>
#include <functional>
#include <memory>

class FormulaAST {
public:
    explicit FormulaAST(std::unique_ptr<Expr> root_expr, std::forward_list<Position> cells);
    FormulaAST(FormulaAST&&) = default;
    FormulaAST& operator=(FormulaAST&&) = default;
    ~FormulaAST();

    double Execute(const SheetInterface& sheet) const;
    void PrintFormula(std::ostream& out) const;
    std::vector<Position> GetReferencedCells() const;

private:
    std::unique_ptr<Expr> root_expr_;
    std::forward_list<Position> cells_;

    class Expr;
    class BinaryOpExpr;
    class UnaryOpExpr;
    class CellExpr;
    class LiteralExpr;
};