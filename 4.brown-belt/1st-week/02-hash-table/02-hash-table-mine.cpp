#include <forward_list>
#include <iterator>
#include <algorithm>

#include "test_runner.h"

using namespace std;

template <typename Type, typename Hasher>
class HashSet {
public:
    using BucketList = forward_list<Type>;

    explicit HashSet(size_t num_buckets, const Hasher& hasher = {}) : data(num_buckets), hasher(hasher) {}
    // Guarantee idempotency by checking if value is already presented
    // hasher(value) % data.size() - index of bucket
    void Add(const Type& value) {
        if (!Has(value))
            data[hasher(value) % data.size()].push_front(value);
    }
    // forward_list supports STL algorithms
    [[nodiscard]] bool Has(const Type& value) const {
        const BucketList& bucket = data[hasher(value) % data.size()];
        return count(begin(bucket), end(bucket), value) > 0;
    }
    // forward_list requires saving previous iterator by hands (why they can't add methods implementations,
    // which will do this by themselves?)
    void Erase(const Type& value) {
        auto& bucket = data[hasher(value) % data.size()];
        for (auto it1 = bucket.before_begin(), it2 = begin(bucket); it2 != end(bucket); it1++, it2++)
            if (*it2 == value) {
                bucket.erase_after(it1);
                break;
            }
    }
    // Just getting bucket by index
    [[nodiscard]] const BucketList& GetBucket(const Type& value) const {
        return data[hasher(value) % data.size()];
    }

private:
    vector<BucketList> data;
    const Hasher hasher;
};
// Tests, provided by authors
struct IntHasher {
    size_t operator()(int value) const {
        return value;
    }
};

struct TestValue {
    int value;

    bool operator==(TestValue other) const {
        return value / 2 == other.value / 2;
    }
};

struct TestValueHasher {
    size_t operator()(TestValue value) const {
        return value.value / 2;
    }
};

void TestSmoke() {
    HashSet<int, IntHasher> hash_set(2);
    hash_set.Add(3);
    hash_set.Add(4);

    ASSERT(hash_set.Has(3))
    ASSERT(hash_set.Has(4))
    ASSERT(!hash_set.Has(5))

    hash_set.Erase(3);

    ASSERT(!hash_set.Has(3))
    ASSERT(hash_set.Has(4))
    ASSERT(!hash_set.Has(5))

    hash_set.Add(3);
    hash_set.Add(5);

    ASSERT(hash_set.Has(3))
    ASSERT(hash_set.Has(4))
    ASSERT(hash_set.Has(5))
}

void TestEmpty() {
    HashSet<int, IntHasher> hash_set(10);
    for (int value = 0; value < 10000; ++value) {
        ASSERT(!hash_set.Has(value))
    }
}

void TestIdempotency() {
    HashSet<int, IntHasher> hash_set(10);
    hash_set.Add(5);
    ASSERT(hash_set.Has(5))
    hash_set.Add(5);
    ASSERT(hash_set.Has(5))
    hash_set.Erase(5);
    ASSERT(!hash_set.Has(5))
    hash_set.Erase(5);
    ASSERT(!hash_set.Has(5))
}

void TestEquivalence() {
    HashSet<TestValue, TestValueHasher> hash_set(10);
    hash_set.Add(TestValue{2});
    hash_set.Add(TestValue{3});

    ASSERT(hash_set.Has(TestValue{2}))
    ASSERT(hash_set.Has(TestValue{3}))

    const auto& bucket = hash_set.GetBucket(TestValue{2});
    const auto& three_bucket = hash_set.GetBucket(TestValue{3});
    ASSERT_EQUAL(&bucket, &three_bucket)

    ASSERT_EQUAL(1, distance(begin(bucket), end(bucket)))
    ASSERT_EQUAL(2, bucket.front().value)
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestSmoke);
    RUN_TEST(tr, TestEmpty);
    RUN_TEST(tr, TestIdempotency);
    RUN_TEST(tr, TestEquivalence);
    return 0;
}
