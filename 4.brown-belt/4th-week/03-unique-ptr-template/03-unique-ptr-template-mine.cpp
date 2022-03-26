#include <cstddef>

#include "test_runner.h"

using namespace std;
// Implementing our own unique_ptr
template <typename T>
class UniquePtr {
public:
    UniquePtr() = default;

    explicit UniquePtr(T* ptr) : pointer(move(ptr)) {}

    UniquePtr(const UniquePtr&) = delete;
    // Do not forget to nullify moved values (I don't really know why)
    UniquePtr(UniquePtr&& other) noexcept {
        pointer = move(other.pointer);
        other.pointer = nullptr;
    }

    UniquePtr& operator = (const UniquePtr&) = delete;
    // And do not forget to return *this
    UniquePtr& operator = (nullptr_t) {
        delete pointer;
        pointer = nullptr;
        return *this;
    }

    UniquePtr& operator = (UniquePtr&& other) noexcept {
        delete pointer;
        pointer = move(other.pointer);
        other.pointer = nullptr;
        return *this;
    }

    ~UniquePtr() {
        delete pointer;
    }

    T& operator * () const {
        return *pointer;
    }

    T* operator -> () const {
        return pointer;
    }

    T* Release() {
        T* tmp = move(pointer);
        pointer = nullptr;
        return move(tmp);
    }

    void Reset(T* ptr) {
        delete pointer;
        pointer = move(ptr);
    }

    void Swap(UniquePtr& other) {
        T* tmp = move(pointer);
        pointer = move(other.pointer);
        other.pointer = move(tmp);
    }

    [[nodiscard]] T* Get() const {
        return pointer;
    }

private:
    T* pointer = nullptr;
};
// Tests, provided by authors
struct Item {
    static int counter;
    int value;
    explicit Item(int v = 0): value(v) {
        ++counter;
    }
    Item(const Item& other): value(other.value) {
        ++counter;
    }
    ~Item() {
        --counter;
    }
};

int Item::counter = 0;

void TestLifetime() {
    Item::counter = 0;
    {
        UniquePtr<Item> ptr(new Item);
        ASSERT_EQUAL(Item::counter, 1)

        ptr.Reset(new Item);
        ASSERT_EQUAL(Item::counter, 1)
    }
    ASSERT_EQUAL(Item::counter, 0)

    {
        UniquePtr<Item> ptr(new Item);
        ASSERT_EQUAL(Item::counter, 1)

        auto rawPtr = ptr.Release();
        ASSERT_EQUAL(Item::counter, 1)

        delete rawPtr;
        ASSERT_EQUAL(Item::counter, 0)
    }
    ASSERT_EQUAL(Item::counter, 0)
}

void TestGetters() {
    UniquePtr<Item> ptr(new Item(42));
    ASSERT_EQUAL(ptr.Get()->value, 42)
    ASSERT_EQUAL((*ptr).value, 42)
    ASSERT_EQUAL(ptr->value, 42)
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestLifetime);
    RUN_TEST(tr, TestGetters);
}
