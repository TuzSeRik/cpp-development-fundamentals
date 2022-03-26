#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>

#include "animals.h"
#include "test_runner.h"

using namespace std;
// As I understand, there is no way to use polymorphism in C++, unless you use pointers to objects
// So we changing type from parent class to pointer to parent class, so objects store pointers to
// overloaded methods and use them, not base class methods
using Zoo = vector<unique_ptr<Animal>>;
// Now store pointers, not just objects
Zoo CreateZoo(istream& in) {
    Zoo zoo;
    string word;
    while (in >> word) {
        if (word == "Tiger") {
            zoo.push_back(make_unique<Tiger>());
        }
        else
        if (word == "Wolf") {
            zoo.push_back(make_unique<Wolf>());
        }
        else
        if (word == "Fox") {
            zoo.push_back(make_unique<Fox>());
        }
        else
            throw runtime_error("Unknown animal!");
    }
    return zoo;
}

void Process(const Zoo& zoo, ostream& out) {
    for (const auto& animal : zoo)
        out << animal->Voice() << "\n";
}
// Tests, provided by authors
void TestZoo() {
    istringstream input("Tiger Wolf Fox Tiger");
    ostringstream output;
    Process(CreateZoo(input), output);

    const string expected =
            "Rrrr\n"
            "Wooo\n"
            "Tyaf\n"
            "Rrrr\n";

    ASSERT_EQUAL(output.str(), expected)
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestZoo);
}
