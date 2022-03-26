#include <algorithm>
#include <iostream>
#include <vector>

#include "json.h"
#include "test_runner.h"

using namespace std;
// Code from lecture
struct Spending {
    string category;
    int amount;
};

bool operator == (const Spending& lhs, const Spending& rhs) {
    return lhs.category == rhs.category && lhs.amount == rhs.amount;
}

ostream& operator << (ostream& os, const Spending& s) {
    return os << '(' << s.category << ": " << s.amount << ')';
}

int CalculateTotalSpendings(
        const vector<Spending>& spendings
) {
    int result = 0;
    for (const Spending& s : spendings) {
        result += s.amount;
    }
    return result;
}

string MostExpensiveCategory(
        const vector<Spending>& spendings
) {
    auto compare_by_amount =
            [](const Spending& lhs, const Spending& rhs) {
                return lhs.amount < rhs.amount;
            };
    return max_element(begin(spendings), end(spendings),
                       compare_by_amount)->category;
}
// Function, we actually implement
vector<Spending> LoadFromJson(istream& input) {
    auto childrenNodes = Load(input).GetRoot().AsArray();
    vector<Spending> result;
    // Getting children of a root, getting map of all properties of every children and pushing them to vector
    result.reserve(childrenNodes.size());
    for(auto& child: childrenNodes)
        result.push_back({child.AsMap().at("category").AsString(),
                          child.AsMap().at("amount").AsInt()});
    // This implementation of parser is much unclear, than previous xml parser
    return result;
}
// Tests, provided by authors
void TestLoadFromJson() {
    istringstream json_input(R"([
    {"amount": 2500, "category": "food"},
    {"amount": 1150, "category": "transport"},
    {"amount": 5780, "category": "restaurants"},
    {"amount": 7500, "category": "clothes"},
    {"amount": 23740, "category": "travel"},
    {"amount": 12000, "category": "sport"}
  ])");

    const vector<Spending> spendings = LoadFromJson(json_input);

    const vector<Spending> expected = {
            {"food", 2500},
            {"transport", 1150},
            {"restaurants", 5780},
            {"clothes", 7500},
            {"travel", 23740},
            {"sport", 12000}
    };
    ASSERT_EQUAL(spendings, expected)
}

void TestJsonLibrary() {
    istringstream json_input(R"([
    {"amount": 2500, "category": "food"},
    {"amount": 1150, "category": "transport"},
    {"amount": 12000, "category": "sport"}
  ])");

    Document doc = Load(json_input);
    const vector<Node>& root = doc.GetRoot().AsArray();
    ASSERT_EQUAL(root.size(), 3u)

    const map<string, Node>& food = root.front().AsMap();
    ASSERT_EQUAL(food.at("category").AsString(), "food")
    ASSERT_EQUAL(food.at("amount").AsInt(), 2500)

    const map<string, Node>& sport = root.back().AsMap();
    ASSERT_EQUAL(sport.at("category").AsString(), "sport")
    ASSERT_EQUAL(sport.at("amount").AsInt(), 12000)

    Node transport(map<string, Node>{{"category", Node("transport")}, {"amount", Node(1150)}});
    Node array_node(vector<Node>{transport});
    ASSERT_EQUAL(array_node.AsArray().size(), 1u)
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestJsonLibrary);
    RUN_TEST(tr, TestLoadFromJson);
}
