#include <numeric>
#include <vector>
#include <string>
#include <future>
#include <mutex>
#include <queue>

#include "test_runner.h"

using namespace std;

template <typename T>
class Synchronized {
public:
    explicit Synchronized(T initial = T()) : value(move(initial)) {}

    struct Access {
    public:
        // Adding a constructor to initialise lock guard
        explicit Access(T& value, mutex& m) : ref_to_value(value), lock(m) {}

        T& ref_to_value;

    private:
        // Lock is active while lock guard in the scope is alive.
        // Since we need to protect field in Access struct, lock guard should be alive all over Access lifetime.
        // That's why we initialise it while constructing and deleting it while destructing Access.
        [[maybe_unused]] lock_guard<mutex> lock;
    };

    struct ConstAccess {
    public:
        // Making referenced value const
        explicit ConstAccess(const T& value, mutex& m) : ref_to_value(value), lock(m) {}

        const T& ref_to_value;

    private:
        [[maybe_unused]] lock_guard<mutex> lock;
    };

    Access GetAccess() {
        return Access{value, m};
    }

    ConstAccess GetAccess() const {
        return ConstAccess{value, m};
    }

private:
    T value;
    // Making mutex mutable, so it can be locked in const method
    mutable mutex m;
};

void TestConcurrentUpdate() {
    Synchronized<string> common_string;

    const size_t add_count = 50000;
    auto updater = [&common_string, add_count] {
        for (size_t i = 0; i < add_count; ++i) {
            auto access = common_string.GetAccess();
            access.ref_to_value += 'a';
        }
    };

    auto f1 = async(updater);
    auto f2 = async(updater);

    f1.get();
    f2.get();

    ASSERT_EQUAL(common_string.GetAccess().ref_to_value.size(), 2 * add_count)
}

vector<int> Consume(Synchronized<deque<int>>& common_queue) {
    vector<int> got;

    for (;;) {
        deque<int> q;

        {
            auto access = common_queue.GetAccess();
            q = move(access.ref_to_value);
        }

        for (int item : q) {
            if (item > 0) {
                got.push_back(item);
            } else {
                return got;
            }
        }
    }
}

void Log(const Synchronized<deque<int>>& common_queue, ostream& out) {
    for (int i = 0; i < 100; ++i) {
        out << "Queue size is " << common_queue.GetAccess().ref_to_value.size() << '\n';
    }
}

void TestProducerConsumer() {
    Synchronized<deque<int>> common_queue;
    ostringstream log;

    auto consumer = async(Consume, ref(common_queue));
    auto logger = async(Log, cref(common_queue), ref(log));

    const size_t item_count = 100000;
    for (size_t i = 1; i <= item_count; ++i) {
        common_queue.GetAccess().ref_to_value.push_back(i);
    }
    common_queue.GetAccess().ref_to_value.push_back(-1);

    vector<int> expected(item_count);
    iota(begin(expected), end(expected), 1);
    ASSERT_EQUAL(consumer.get(), expected)

    logger.get();
    const string logs = log.str();
    ASSERT(!logs.empty())
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestConcurrentUpdate);
    RUN_TEST(tr, TestProducerConsumer);
}
