#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <optional>

using namespace std;
// Why anyone not added same concept to the STL?
template <typename Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator begin, Iterator end) : first(begin), last(end) {}

    [[nodiscard]] Iterator begin() const {
        return first;
    }

    [[nodiscard]] Iterator end() const {
        return last;
    }

private:
    Iterator first, last;
};
// Supplemental code
struct Person {
    string name;
    int age, income;
    bool is_male;
};

vector<Person> ReadPeople(istream& input) {
    int count;
    input >> count;

    vector<Person> result(count);
    for (Person& p : result) {
        char gender;
        input >> p.name >> p.age >> p.income >> gender;
        p.is_male = gender == 'M';
    }

    return result;
}
// My actual code
// List of people is not changing through commands, so we can pre-calculate all needed values
struct Statistics {
    optional<string> mostPopularMaleName;
    optional<string> mostPopularFemaleName;
    vector<int> accumulatedWealth;
    vector<Person> sortedPeople;
};
// Finding most popular name in range (of males or females)
template<typename It>
optional<string> findMostPopularName(IteratorRange<It> range) {
    if (begin(range) == end(range))
        return nullopt;

    sort(begin(range), end(range), [](const Person& lhs, const Person& rhs) {
        return lhs.name < rhs.name;
    });
    // Counting names in blocks and if name has bigger count or is bigger than saved, saving it
    const string* mostPopularName = &begin(range)->name;
    uint count = 1;
    for (auto it = begin(range); it != end(range);) {
        auto endOfNameBlock = find_if_not(it, end(range), [it](const Person& p) {
            return p.name == it->name;
        });

        const auto nameCount = distance(it, endOfNameBlock);
        if (nameCount > count || (nameCount == count && it->name < *mostPopularName)) {
            count = nameCount;
            mostPopularName = &it->name;
        }

        it = endOfNameBlock;
    }

    return *mostPopularName;
}
// Pre-calculating all needed data
Statistics buildStatistics(vector<Person>&& people) {
    Statistics result;
    // Looking for popular names for males and females
    IteratorRange males{begin(people),
                        partition(begin(people), end(people), [](const Person &p) {
                            return p.is_male;
                        })
    };
    IteratorRange females{males.end(), end(people)};

    result.mostPopularMaleName = findMostPopularName(males);
    result.mostPopularFemaleName = findMostPopularName(females);
    // Sorting people by wealth and accumulate wealth of all previous persons in vector
    sort(people.begin(), people.end(), [](const Person &lhs, const Person &rhs) {
        return lhs.income > rhs.income;
    });

    result.accumulatedWealth.resize(people.size());
    if (!people.empty()) {
        result.accumulatedWealth[0] = people[0].income;
        for (size_t i = 1; i < people.size(); ++i)
            result.accumulatedWealth[i] = result.accumulatedWealth[i - 1] + people[i].income;
    }

    // Sorting people by age
    sort(begin(people), end(people), [](const Person &lhs, const Person &rhs) {
        return lhs.age < rhs.age;
    });
    result.sortedPeople = move(people);

    return result;
}

int main() {
    // Calculating statistics
    const Statistics statistics = buildStatistics(ReadPeople(cin));
    // Working with commands
    for (string command; cin >> command;) {
        if (command == "AGE") {
            int adult_age;
            cin >> adult_age;
            // Now we calculate only dynamically changing parameters
            const auto adult_begin = lower_bound(
                    begin(statistics.sortedPeople), end(statistics.sortedPeople), adult_age,
                    [](const Person &lhs, int age) {
                        return lhs.age < age;
                    }
            );

            cout << "There are " << distance(adult_begin, end(statistics.sortedPeople))
                 << " adult people for maturity age " << adult_age << '\n';
        }
            // And not calculating values, that not change through program lifetime
        else if (command == "WEALTHY") {
            int count;
            cin >> count;

            cout << "Top-" << count << " people have total income "
                 << statistics.accumulatedWealth[count - 1] << '\n';
        }

        else if (command == "POPULAR_NAME") {
            char gender;
            cin >> gender;

            const auto& mostPopularName =
                    gender == 'M' ? statistics.mostPopularMaleName :
                    statistics.mostPopularFemaleName;

            if (mostPopularName)
                cout << "Most popular name among people of gender " << gender << " is "
                     << *mostPopularName << '\n';
            else
                cout << "No people of gender " << gender << '\n';
        }
    }
}
