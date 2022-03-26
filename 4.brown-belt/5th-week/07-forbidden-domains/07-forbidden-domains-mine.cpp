#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <string_view>
#include <algorithm>

#include "test_runner.h"

using namespace std;
// Supplemental representation of Domain structure and methods and functions
class Domain {
public:
    explicit Domain(const string& domains);

    [[nodiscard]] bool IsSubDomain(const Domain& sub_domain) const;

    vector<string> domain_tokens;

private:
    static vector<string> split(const string& domains);

    static vector<string> reverse(const vector<string>& tokens);
};

class DomainsFilter {
public:
    void AddForbiddenDomain(const string& domain);

    [[nodiscard]] bool CheckDomain(const string& domain) const;

private:
    unordered_map<std::string, vector<Domain>> domains_;

    static string getDomainHead(string_view domain);
};

void PerformDomainsQueries(istream& input = cin, ostream& output = cout);

vector<string> Domain::split(const string& domains) {
    vector<string> result;

    string token;
    for (const auto& ch : domains) {
        if (ch == '.') {
            result.push_back(token);
            token = "";
        }
        else
            token += ch;
    }
    result.push_back(token);

    return result;
}

vector<string> Domain::reverse(const vector<string>& tokens) {
    vector<std::string> result;
    result.reserve(tokens.size());

    for (auto token = tokens.rbegin(); token != tokens.rend(); ++token)
        result.push_back(*token);

    return result;
}

bool Domain::IsSubDomain(const Domain& sub_domain) const {
    if (domain_tokens.size() < sub_domain.domain_tokens.size())
        return false;

    size_t i = 0;
    for (const auto& token : sub_domain.domain_tokens) {
        if (token != domain_tokens.at(i))
            return false;

        ++i;
    }

    return true;
}

Domain::Domain(const string& domains) {
    domain_tokens = reverse(split(domains));
}

string DomainsFilter::getDomainHead(string_view domain) {
    const auto pos = domain.find_last_of('.');

    if (pos == string_view::npos)
        return string(domain);

    return string(domain.substr(pos + 1, domain.size() - pos));
}

void DomainsFilter::AddForbiddenDomain(const string& domain) {
    domains_[getDomainHead(domain)].push_back(Domain(domain));
}

bool DomainsFilter::CheckDomain(const string& domain) const {
    if (domains_.count(getDomainHead(domain)) == 0)
        return false;

    const Domain checking_domain = Domain(domain);

    const auto& domains = domains_.at(getDomainHead(domain));

    for (const Domain& domain_ : domains)
        if (checking_domain.IsSubDomain(domain_))
            return true;

    return false;
}
// Main control function
void PerformDomainsQueries(istream& input, ostream& output) {
    DomainsFilter filter;

    int n;
    input >> n;
    string domain;
    for (int i = 0; i < n; ++i) {
        input >> domain;
        filter.AddForbiddenDomain(domain);
    }

    int m;
    input >> m;

    for (int i = 0; i < m; ++i) {
        input >> domain;

        if (filter.CheckDomain(domain))
            output << "Bad";
        else
            output << "Good";

        output << '\n';
    }
}
// Tests, provided by authors
void TestDomainsFilter() {
    DomainsFilter filter;

    filter.AddForbiddenDomain("ya.ru");
    filter.AddForbiddenDomain("maps.me");
    filter.AddForbiddenDomain("m.ya.ru");
    filter.AddForbiddenDomain("com");

    ASSERT(filter.CheckDomain("ya.ru"))
    ASSERT(filter.CheckDomain("ya.com"))
    ASSERT(filter.CheckDomain("m.maps.me"))
    ASSERT(filter.CheckDomain("moscow.m.ya.ru"))
    ASSERT(filter.CheckDomain("maps.com"))
    ASSERT(!filter.CheckDomain("maps.ru"))
    ASSERT(!filter.CheckDomain("ya.ya"))
    ASSERT(filter.CheckDomain("com"))
    ASSERT(!filter.CheckDomain("mmaps.me"))
    ASSERT(!filter.CheckDomain("mamya.ru"))
}

void TestPerformDomainsQueries() {
    stringstream input;
    input << "4\n" << "ya.ru\n" << "maps.me\n" << "m.ya.ru\n" << "com\n" << "7\n" << "ya.ru\n" << "ya.com\n";
    input << "m.maps.me\n" << "moscow.m.ya.ru\n" << "maps.com\n" << "maps.ru\n" << "ya.ya\n";

    std::stringstream output;
    PerformDomainsQueries(input, output);
    string expected =
            "Bad\n"
            "Bad\n"
            "Bad\n"
            "Bad\n"
            "Bad\n"
            "Good\n"
            "Good\n";

    ASSERT_EQUAL(output.str(), expected)
}

int main() {
    //TestRunner tr;
    // Commenting tests, to get some more efficiency
    //RUN_TEST(tr, TestDomainsFilter);
    //RUN_TEST(tr, TestPerformDomainsQueries);

    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    PerformDomainsQueries();
}
