#include <functional>
#include <string>

#include "test_runner.h"

using namespace std;

template <typename T>
class LazyValue {
public:
    explicit LazyValue(function<T()> init) : initializer(move(init)) {}
    // Using standard optional<> method
    [[nodiscard]] bool HasValue() const {
        return lazyValue.has_value();
    }
    // Simple lazy initialisation
    [[nodiscard]] const T& Get() const {
        if (!lazyValue.has_value())
            lazyValue = initializer();
        return *lazyValue;
    }
// Storing constructor and optional<> value
private:
    function<T()> initializer;
    mutable optional<T> lazyValue;
};
// Tests, provided by authors and one by me
void UseExample() {
    const string big_string = "Giant amounts of memory";

    LazyValue<string> lazy_string([&big_string] { return big_string; });

    ASSERT(!lazy_string.HasValue())
    ASSERT_EQUAL(lazy_string.Get(), big_string)
    ASSERT_EQUAL(lazy_string.Get(), big_string)
    // Test, added by me, to check if value assigned not to pointer (because if assigned by *lazyValue = initializer()
    // *lazyValue returns value and lazyValue.has_value() returns false
    ASSERT_EQUAL(lazy_string.HasValue(), true)
}

void TestInitializerIsntCalled() {
    bool called = false;

    {
        LazyValue<int> lazy_int([&called] {
            called = true;
            return 0;
        });
    }
    ASSERT(!called)
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, UseExample);
    RUN_TEST(tr, TestInitializerIsntCalled);
    return 0;
}
