#include <iostream>
#include <string>
#include <map>
#include <list>

#include "test_runner.h"

using namespace std;
// Record type. It can be easily extended with only few additions to the program
struct Record {
    string id;
    string title;
    string user;
    int timestamp;
    int karma;
};

class Database {
public:
    bool Put(const Record& record) {
        if (recordMap.find(record.id) == end(recordMap)) {
            // Storing data in list
            data.push_front(record);
            auto recordIt = begin(data);
            recordMap.insert({record.id, recordIt});
            // Storing iterators with key to sort in one group to simplify erasing
            auto group = iteratorsMap.insert({record.id,
                                              {timestampIndex.insert({record.timestamp, recordIt}),
                                               karmaIndex.insert({record.karma, recordIt}),
                                               userIndex.insert({record.user, recordIt})}});

            return true;
        }
        else
            return false;
    }

    const Record* GetById(const string& id) const {
        // Using exceptions to prevent spending log N time on find
        try {
            return &*(recordMap.at(id));
        }
        catch (out_of_range&) {
            return nullptr;
        }
    }

    bool Erase(const string& id) {
        auto it = recordMap.find(id);

        if (it != end(recordMap)) {
            // Erasing actual data
            data.erase(it->second);
            recordMap.erase(it);
            // Erasing invalidated iterators to it in secondary indexes
            auto groupIterator = iteratorsMap.at(id);
            timestampIndex.erase(groupIterator.timestampIndexIterator);
            karmaIndex.erase(groupIterator.karmaIndexIterator);
            userIndex.erase(groupIterator.userIndexIterator);

            return true;
        }
        else
            return false;
    }
    // Inner template function can map data in any row if it stored in indexes like these
    template <typename Callback>
    void RangeByTimestamp(int low, int high, Callback callback) const {
        mappingByFieldValue(timestampIndex, low, high, callback);
    }

    template <typename Callback>
    void RangeByKarma(int low, int high, Callback callback) const {
        mappingByFieldValue(karmaIndex, low, high, callback);
    }

    template <typename Callback>
    void AllByUser(const string& user, Callback callback) const {
        mappingByFieldValue(userIndex, user, user, callback);
    }

private:
    // Storing data in list and only iterators to it in other containers
    list<Record> data;
    using Iterator = list<Record>::iterator;
    unordered_map<string, Iterator> recordMap;
    // Naming iterators fields for simplicity
    struct IteratorsGroup {
        multimap<int, Iterator>::iterator timestampIndexIterator, karmaIndexIterator;
        multimap<string, Iterator>::iterator userIndexIterator;
    };

    unordered_map<string, IteratorsGroup> iteratorsMap;
    multimap<int, Iterator> timestampIndex, karmaIndex;
    multimap<string, Iterator> userIndex;
    // Pretty universal mapping function for any container, storing <Key, Value> pair
    // and having lower_bound and upper_bound functions
    template<template<typename, typename...> typename Container, typename K, typename Callback>
    void mappingByFieldValue(const Container<K, Iterator>& index, const K& low, const K& high, Callback& callback) const {
        // Now I know that you NEVER should use conditions with calculation complexity more than O(1)
        const auto end = index.upper_bound(high);
        for (auto it = index.lower_bound(low); it != end; it++)
            if (!callback(*(it->second)))
                break;
    }
};
// Tests, provided by authors
void TestRangeBoundaries() {
    const int good_karma = 1000;
    const int bad_karma = -10;

    Database db;
    db.Put({"id1", "Hello there", "master", 1536107260, good_karma});
    db.Put({"id2", "O>>-<", "general2", 1536107260, bad_karma});

    int count = 0;
    db.RangeByKarma(bad_karma, good_karma, [&count](const Record&) {
        ++count;
        return true;
    });

    ASSERT_EQUAL(2, count)
}

void TestSameUser() {
    Database db;
    db.Put({"id1", "Don't sell", "master", 1536107260, 1000});
    db.Put({"id2", "Rethink life", "master", 1536107260, 2000});

    int count = 0;
    db.AllByUser("master", [&count](const Record&) {
        ++count;
        return true;
    });

    ASSERT_EQUAL(2, count)
}

void TestReplacement() {
    const string final_body = "Feeling sad";

    Database db;
    db.Put({"id", "Have a hand", "not-master", 1536107260, 10});
    db.Erase("id");
    db.Put({"id", final_body, "not-master", 1536107260, -10});

    auto record = db.GetById("id");
    ASSERT(record != nullptr)
    ASSERT_EQUAL(final_body, record->title)
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestRangeBoundaries);
    RUN_TEST(tr, TestSameUser);
    RUN_TEST(tr, TestReplacement);
    return 0;
}
