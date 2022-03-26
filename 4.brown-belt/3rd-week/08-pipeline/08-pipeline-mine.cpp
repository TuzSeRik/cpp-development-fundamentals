#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

#include "test_runner.h"

using namespace std;

struct Email {
    string from;
    string to;
    string body;
};
// Base class of all elements of pipeline
class Worker {
public:
    virtual ~Worker() = default;
    // Main pipeline element's work method
    virtual void Process(unique_ptr<Email> email) = 0;

    virtual void Run() {
        throw logic_error("Unimplemented");
    }
    // Main pipeline element's connect method
    void SetNext(unique_ptr<Worker> next) {
        this->next = move(next);
    }

    unique_ptr<Worker> next;

protected:
    void PassOn(unique_ptr<Email> email) const {
        if (next)
            next->Process(move(email));
    }
};
// Operator needed to prevent corrupted emails to proceed (not stated condition, but needed)
istream& operator >>(istream& input, Email& email) {
    string from, to, body;

    getline(input, from);
    getline(input, to);
    getline(input, body);

    email = Email{move(from), move(to), move(body)};

    return input;
}

class Reader : public Worker {
public:
    explicit Reader(istream& input) : input(input) {}
    // I think it's a check some sort - no other classes use Run
    // and this is the only one, which have no description of Process
    void Process(unique_ptr<Email> email) override {
        Run();
    }
    // Reading emails and sending them to next element
    void Run() override {
        Email email;
        while (input >> email)
            this->PassOn(move(make_unique<Email>(email)));
    }

private:
    istream& input;
};

class Filter : public Worker {
public:
    using Function = function<bool(const Email&)>;

    explicit Filter(Function function) : predicate(move(function)) {}
    // Checking email by predicate and proceeding that fit
    void Process(unique_ptr<Email> email) override {
        if (predicate(*email))
            PassOn(move(email));
    }

private:
    Function predicate;
};

class Copier : public Worker {
public:
    explicit Copier(string to) : to(move(to)) {}

    void Process(unique_ptr<Email> email) override {
        //Checking if copy is needed. Copied email should always go after main one
        if (email->to != this->to) {
            auto copiedEmail = make_unique<Email>();
            copiedEmail->from = email->from;
            copiedEmail->to = this->to;
            copiedEmail->body = email->body;

            PassOn(move(email));
            PassOn(move(copiedEmail));
        }
        else
            PassOn(move(email));
    }

private:
    string to;
};

class Sender : public Worker {
public:
    explicit Sender(ostream& output) : output(output) {}
    // We don't need to check stream, so we don't need operator for output
    void Process(unique_ptr<Email> email) override {
        output << email->from << "\n";
        output << email->to << "\n";
        output << email->body << "\n";

        PassOn(move(email));
    }

private:
    ostream& output;
};

class PipelineBuilder {
public:
    // To save time on operations with container (pushing elements to them and constructing resulted pipeline)
    // we store first element in pipeline and simple pointer to last element to pipeline
    explicit PipelineBuilder(istream& in) : begin(make_unique<Reader>(in)), current(begin.get()) {}
    // Then we need only to refresh pointer after adding element
    PipelineBuilder& FilterBy(Filter::Function filter) {
        current->SetNext(make_unique<Filter>(move(filter)));
        current = current->next.get();

        return *this;
    }

    PipelineBuilder& CopyTo(string recipient) {
        current->SetNext(make_unique<Copier>(move(recipient)));
        current = current->next.get();

        return *this;
    }

    PipelineBuilder& Send(ostream& out) {
        current->SetNext(make_unique<Sender>(out));
        current = current->next.get();

        return *this;
    }
    // And simply return first element, which takes O(1)
    unique_ptr<Worker> Build() {
        return move(begin);
    }

private:
    unique_ptr<Worker> begin;
    Worker* current;
};
// Tests, provided by authors
void TestSanity() {
    string input = (
            "erich@example.com\n"
            "richard@example.com\n"
            "Hello there\n"

            "erich@example.com\n"
            "ralph@example.com\n"
            "Are you sure you pressed the right button?\n"

            "ralph@example.com\n"
            "erich@example.com\n"
            "I do not make mistakes of that kind\n"
    );
    istringstream inStream(input);
    ostringstream outStream;

    PipelineBuilder builder(inStream);
    builder.FilterBy([](const Email& email) {
        return email.from == "erich@example.com";
    });
    builder.CopyTo("richard@example.com");
    builder.Send(outStream);
    auto pipeline = builder.Build();

    pipeline->Run();

    string expectedOutput = (
            "erich@example.com\n"
            "richard@example.com\n"
            "Hello there\n"

            "erich@example.com\n"
            "ralph@example.com\n"
            "Are you sure you pressed the right button?\n"

            "erich@example.com\n"
            "richard@example.com\n"
            "Are you sure you pressed the right button?\n"
    );
    auto output = outStream.str();
    ASSERT_EQUAL(expectedOutput, outStream.str())
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestSanity);
    return 0;
}
