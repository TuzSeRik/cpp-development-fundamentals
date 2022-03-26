#include <unordered_map>
#include <map>
#include <optional>
#include <vector>
#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

class Date {
public:
    Date();
    explicit Date(const std::string& input_date);

    [[nodiscard]] int GetYear() const;
    [[nodiscard]] int GetMonth() const;
    [[nodiscard]] int GetDay() const;

    [[nodiscard]] string GetDate(char separator = '-') const;

    [[nodiscard]] time_t AsTimestamp() const;

    static int ComputeDaysDiff(const Date& dateTo, const Date& dateFrom);

private:
    int year{};
    int month{};
    int day{};

    void CheckFormat(const string& input);
    static bool CheckNumber(stringstream& ss, int& number);
    void CheckData() const;
};

bool operator<(const Date& lhs, const Date& rhs);

bool operator==(const Date& lhs, const Date& rhs);

class Budget {
public:
    Budget() = default;

    double ComputeTotalIncome(const Date& from, const Date& to);

    void SetIncome(const Date& from, const Date& to, double value);

    void PayTax(const Date &from, const Date &to, double tax);

    void Spend(const Date& from, const Date& to, double value);

private:
    static const int SECONDS_IN_DAY = 60 * 60 * 24;
    struct Day {
        [[nodiscard]] double ComputeIncome() const {
            return income - expended;
        }

        void PayTax(double tax) {
            income *= 1 - tax/100.0;
        }

        double income;
        double expended;
    };

    unordered_map<time_t, Day> incomeForDay;
};

class BudgetManager {
public:
    explicit BudgetManager(istream& stream) : stream(stream) {}

    vector<double> Process();

    BudgetManager() = delete;

private:
    Budget budget;
    istream& stream;
};

void PrintResult(const std::vector<double>& result, std::ostream& stream);

Date::Date() {
    year = 0;
    month = 0;
    day = 0;
}

Date::Date(const std::string &input_date) {
    CheckFormat(input_date);
    CheckData();
}

int Date::GetYear() const { return year; }

int Date::GetMonth() const { return month; }

int Date::GetDay() const { return day; }

string Date::GetDate(char separator) const {
    stringstream so;
    so << setfill('0');
    if (year < 0) {
        so << '-';
    }
    so << setw(4) << abs(year);
    so << separator;
    so << setw(2) << month;
    so << separator;
    so << setw(2) << day;
    return so.str();
}

time_t Date::AsTimestamp() const {
    tm t{};
    t.tm_sec   = 0;
    t.tm_min   = 0;
    t.tm_hour  = 0;
    t.tm_mday  = day;
    t.tm_mon   = month - 1;
    t.tm_year  = year - 1900;
    t.tm_isdst = 0;
    return mktime(&t);
}

int Date::ComputeDaysDiff(const Date& dateTo, const Date& dateFrom) {
    const time_t timestampTo = dateTo.AsTimestamp();
    const time_t timestampFrom = dateFrom.AsTimestamp();
    static const int SECONDS_IN_DAY = 60 * 60 * 24;
    return (timestampTo - timestampFrom) / SECONDS_IN_DAY;
}

void Date::CheckFormat(const string &input) {
    stringstream ss(input);
    int countData = 3;
    int dateData[3] = {-1, -1, -1};
    year = dateData[0];
    month = dateData[1];
    day = dateData[2];

    if (!CheckNumber(ss, dateData[0])) {
        stringstream so;
        so << "Wrong date format: " << input;
        throw runtime_error(so.str());
    }

    char inputDateSeparator = '-';
    for (int i = 1; i < countData; ++i) {
        if (ss.peek() != inputDateSeparator) {
            stringstream so;
            so << "Wrong date format: " << input;
            throw runtime_error(so.str());
        }
        else {
            ss.ignore(1);
            if (!CheckNumber(ss, dateData[i])) {
                stringstream so;
                so << "Wrong date format: " << input;
                throw runtime_error(so.str());
            }
        }
    }

    if (ss.rdbuf()->in_avail() != 0) {
        stringstream so;
        so << "Wrong date format: " << input;
        throw runtime_error(so.str());
    }
    year = dateData[0];
    month = dateData[1];
    day = dateData[2];
}

bool Date::CheckNumber(stringstream& ss, int& number) {
    char sign = '+';
    if (ss.peek() == '+' || ss.peek() == '-') {
        sign = ss.get();
    }
    if (ss.peek() >= '0' && ss.peek() <= '9') {
        ss >> number;
        if (sign == '-')
            number = -number;
    }
    else
        return false;
    return true;
}

void Date::CheckData() const {
    if (month < 1 || month > 12) {
        stringstream ss;
        ss << "Month value is invalid: " << month;
        throw runtime_error(ss.str());
    }

    if (day < 1 || day > 31) {
        stringstream ss;
        ss << "Day value is invalid: " << day;
        throw runtime_error(ss.str());
    }
}

bool operator<(const Date& lhs, const Date& rhs) {
    int l_year = lhs.GetYear(), l_month = lhs.GetMonth(), l_day = lhs.GetDay();
    int r_year = rhs.GetYear(), r_month = rhs.GetMonth(), r_day = rhs.GetDay();
    return (tie(l_year, l_month, l_day) < tie(r_year, r_month, r_day));
}

bool operator==(const Date& lhs, const Date& rhs) {
    return (lhs.GetYear() == rhs.GetYear()
            && lhs.GetMonth() == rhs.GetMonth()
            && lhs.GetDay() == rhs.GetDay());
}

double Budget::ComputeTotalIncome(const Date &from, const Date &to) {
    auto timeFrom = from.AsTimestamp();
    auto timeTo = to.AsTimestamp();

    double res = 0;
    for (auto currDay = timeFrom; currDay <= timeTo; currDay += SECONDS_IN_DAY) {
        res += incomeForDay[currDay].ComputeIncome();
    }

    return res;
}

void Budget::PayTax(const Date &from, const Date &to, double tax) {
    auto timeFrom = from.AsTimestamp();
    auto timeTo = to.AsTimestamp();
    for (auto currDay = timeFrom; currDay <= timeTo; currDay += SECONDS_IN_DAY) {
        incomeForDay[currDay].PayTax(tax);
    }
}

void Budget::SetIncome(const Date &from, const Date &to, double value) {
    auto timeFrom = from.AsTimestamp();
    auto timeTo = to.AsTimestamp();
    auto diff = Date::ComputeDaysDiff(to, from) + 1;
    double valuePerDay = value / diff;
    for (auto currDay = timeFrom; currDay <= timeTo; currDay += SECONDS_IN_DAY) {
        incomeForDay[currDay].income += valuePerDay;
    }
}

void Budget::Spend(const Date &from, const Date &to, double value) {
    auto timeFrom = from.AsTimestamp();
    auto timeTo = to.AsTimestamp();
    auto diff = Date::ComputeDaysDiff(to, from) + 1;
    double valuePerDay = value / diff;

    for (auto currDay = timeFrom; currDay <= timeTo; currDay += SECONDS_IN_DAY) {
        incomeForDay[currDay].expended += valuePerDay;
    }
}

vector<double> BudgetManager::Process() {
    vector<double> result;
    size_t count = 0;
    stream >> count;

    for (size_t i = 0; i < count; ++i) {
        string command;
        stream >> command;
        if (command == "Earn") {
            string from, to;
            double value = 0;
            stream >> from >> to >> value;
            budget.SetIncome(Date(from), Date(to), value);
        }
        if (command == "PayTax") {
            string from, to;
            double value;
            stream >> from >> to >> value;
            budget.PayTax(Date(from), Date(to), value);
        }
        if (command == "ComputeIncome") {
            string from, to;
            stream >> from >> to;
            result.push_back(budget.ComputeTotalIncome(Date(from), Date(to)));
        }
        if (command == "Spend") {
            string from, to;
            double value = 0;
            stream >> from >> to >> value;
            budget.Spend(Date(from), Date(to), value);
        }
    }

    return result;
}

void PrintResult(const vector<double>& result, ostream& stream) {
    for (const auto& elem : result)
        stream << elem << endl;
}

int main() {
    cout.precision(25);
    PrintResult(BudgetManager(cin).Process(), cout);
    return 0;
}
