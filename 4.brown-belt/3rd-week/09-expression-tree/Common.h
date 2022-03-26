#pragma once

#include <memory>
#include <string>

class Expression {
public:
    virtual ~Expression() = default;

    [[nodiscard]] virtual int Evaluate() const = 0;

    [[nodiscard]] virtual std::string ToString() const = 0;
};

using ExpressionPtr = std::unique_ptr<Expression>;

ExpressionPtr Value(int value);

ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right);

ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right);
