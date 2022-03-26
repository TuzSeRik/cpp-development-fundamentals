#include <iostream>
#include <algorithm>
#include <vector>
#include <set>

#include "test_runner.h"

using namespace std;
// Actually, this task was already presented somewhere in Red Belt course, but I re-done it here
template <typename T>
class PriorityCollection {
public:
    using Id = size_t;

    Id Add(T object) {
        dataAndPriorityById.emplace_back(move(object), 0);
        priorityAndId.insert({0, dataAndPriorityById.size() - 1});

        return dataAndPriorityById.size() - 1;
    }

    template <typename ObjInputIt, typename IdOutputIt>
    void Add(ObjInputIt range_begin, ObjInputIt range_end,
             IdOutputIt ids_begin) {
        for(auto it = range_begin; it != range_end; it++)
            *ids_begin++ = Add(move(*it));
    }
    // There are two situations where id is invalid - it is not initialised (out_of_range situation)
    // or value already deleted, so id is invalidated by setting priority -1;
    [[nodiscard]] bool IsValid(Id id) const {
        try {
            return dataAndPriorityById[id].second != -1;
        }
        catch (out_of_range&) {
            return false;
        }
    }

    [[nodiscard]] const T& Get(Id id) const {
        return dataAndPriorityById[id].first;
    }
    // Because of this I think that current data structure is not optimal, but the only alternative I see
    // is to sort vector after each promotion and it looks like not so fast as it now
    void Promote(Id id) {
        priorityAndId.erase({dataAndPriorityById[id].second++, id});
        priorityAndId.insert({dataAndPriorityById[id].second, id});
    }

    [[nodiscard]] pair<const T&, int> GetMax() const {
        return dataAndPriorityById[priorityAndId.rbegin()->second];
    }
    // Invalidate entry in vector by moving data and setting priority to -1.
    // Then erasing entry in set and returning moved pair
    pair<T, int> PopMax() {
        auto entry = move(dataAndPriorityById[priorityAndId.rbegin()->second]);
        dataAndPriorityById[priorityAndId.rbegin()->second].second = -1;
        priorityAndId.erase(*priorityAndId.rbegin());
        return entry;
    }

private:
    vector<pair<T, int>> dataAndPriorityById;
    set<pair<int, Id>> priorityAndId;
};
// Tests, provided by authors
class StringNonCopyable : public string {
public:
    using string::string;
    StringNonCopyable(const StringNonCopyable&) = delete;
    StringNonCopyable(StringNonCopyable&&) = default;
    StringNonCopyable& operator=(const StringNonCopyable&) = delete;
    StringNonCopyable& operator=(StringNonCopyable&&) = default;
};

void TestNoCopy() {
    PriorityCollection<StringNonCopyable> strings;
    const auto white_id = strings.Add("white");
    const auto yellow_id = strings.Add("yellow");
    const auto red_id = strings.Add("red");

    strings.Promote(yellow_id);
    for (int i = 0; i < 2; ++i) {
        strings.Promote(red_id);
    }
    strings.Promote(yellow_id);
    {
        const auto item = strings.PopMax();
        ASSERT_EQUAL(item.first, "red");
        ASSERT_EQUAL(item.second, 2);
    }
    {
        const auto item = strings.PopMax();
        ASSERT_EQUAL(item.first, "yellow");
        ASSERT_EQUAL(item.second, 2);
    }
    {
        const auto item = strings.PopMax();
        ASSERT_EQUAL(item.first, "white");
        ASSERT_EQUAL(item.second, 0);
    }
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestNoCopy);
    return 0;
}
