#include <sstream>

#include "Common.h"
#include "test_runner.h"

using namespace std;
// Created classes
namespace Expressions {
    // Value expression is pretty obvious and simple
    class Value : public Expression {
    public:
        explicit Value(int value) : value(value) {}

        [[nodiscard]] int Evaluate() const override {
            return value;
        }

        [[nodiscard]] string ToString() const override {
            return to_string(value);
        }

    private:
        int value;
    };
    // Base class for product and sum
    class Binary : public Expression {
    public:
        Binary(ExpressionPtr leftOp, ExpressionPtr rightOp) :
                leftOperand(move(leftOp)), rightOperand(move(rightOp)) {}

    protected:
        ExpressionPtr leftOperand, rightOperand;
    };
    // Pretty obvious too
    class Sum : public Binary {
    public:
        // Needed to not implement constructor
        using Binary::Binary;

        [[nodiscard]] int Evaluate() const override {
            return leftOperand->Evaluate() + rightOperand->Evaluate();
        }

        [[nodiscard]] string ToString() const override {
            return '(' + leftOperand->ToString() + ')' + '+' + '(' + rightOperand->ToString() + ')';
        }
    };
    // Differs from sum only by sign
    class Product : public Binary {
    public:
        using Binary::Binary;

        [[nodiscard]] int Evaluate() const override {
            return leftOperand->Evaluate() * rightOperand->Evaluate();
        }

        [[nodiscard]] string ToString() const override {
            return '(' + leftOperand->ToString() + ')' + '*' + '(' + rightOperand->ToString() + ')';
        }
    };
}
// Implemented functions. Just returning pointer to created node
ExpressionPtr Value(int value) {
    return make_unique<Expressions::Value>(value);
}

ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right) {
    return make_unique<Expressions::Sum>(move(left), move(right));
}

ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right) {
    return make_unique<Expressions::Product>(move(left), move(right));
}
// Tests, provided by authors
string Print(const Expression* e) {
    if (!e) {
        return "Null expression provided";
    }
    stringstream output;
    output << e->ToString() << " = " << e->Evaluate();
    return output.str();
}

void Test() {
    ExpressionPtr e1 = Product(Value(2), Sum(Value(3), Value(4)));
    ASSERT_EQUAL(Print(e1.get()), "(2)*((3)+(4)) = 14")

    ExpressionPtr e2 = Sum(move(e1), Value(5));
    ASSERT_EQUAL(Print(e2.get()), "((2)*((3)+(4)))+(5) = 19")

    ASSERT_EQUAL(Print(e1.get()), "Null expression provided")
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, Test);
    return 0;
}
