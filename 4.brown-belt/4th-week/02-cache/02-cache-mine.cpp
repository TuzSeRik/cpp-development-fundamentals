#include <unordered_map>
#include <map>
#include <mutex>

#include "Common.h"

using namespace std;
// Implementation of a solution
class LruCache : public ICache {
public:
    LruCache(shared_ptr<IBooksUnpacker> booksUnpacker,
             const Settings& settings) : booksUnpacker(move(booksUnpacker)) {
        freeMemory = settings.max_memory;
        priority = 0;
    }

    BookPtr GetBook(const string& bookName) override {
        m.lock();
        unordered_node node = nameToBook.extract(bookName);
        BookPtr result;

        if (!node.empty())
            result = getExistingBook(node);
        else
            result = getNewBook(bookName);

        return [&]{
            m.unlock();
            return result;
        }();
    }

private:
    shared_ptr<IBooksUnpacker> booksUnpacker;
    size_t freeMemory;
    int priority;
    mutex m = mutex();

    map<int, string> priorityToBookName;
    unordered_map<string, pair<int, BookPtr>> nameToBook;

    using unordered_node = unordered_map<string, pair<int, BookPtr>>::node_type;

    BookPtr getExistingBook(unordered_node& node) {
        BookPtr result = node.mapped().second;

        auto mapNode = priorityToBookName.extract(node.mapped().first);
        mapNode.key() = ++priority;
        priorityToBookName.insert(move(mapNode));

        node.mapped().first = priority;
        nameToBook.insert(move(node));

        return result;
    }

    BookPtr getNewBook(const string& bookName) {
        BookPtr newBook = booksUnpacker->UnpackBook(bookName);

        const size_t wantedMemory = newBook->GetContent().size();

        if (freeMemory >= wantedMemory)
            insertNewBook(newBook, wantedMemory);
        else
        if (freeCacheMemory(wantedMemory))
            insertNewBook(newBook, wantedMemory);

        return newBook;
    }

    void insertNewBook(const BookPtr& newBook, size_t wantedMemory) {
        freeMemory -= wantedMemory;
        priorityToBookName.insert({++priority, newBook->GetName() });
        nameToBook.insert({newBook->GetName(), make_pair(priority, newBook)});
    }

    bool freeCacheMemory(size_t wantedMemory) {
        while (!priorityToBookName.empty()) {
            const auto begin = priorityToBookName.begin();

            const auto finder =  nameToBook.find(begin->second);
            freeMemory += finder->second.second->GetContent().size();

            nameToBook.erase(finder);
            priorityToBookName.erase(begin);

            if (freeMemory >= wantedMemory)
                break;
        }

        return freeMemory >= wantedMemory;
    }
};

unique_ptr<ICache> MakeCache(shared_ptr<IBooksUnpacker> booksUnpacker, const ICache::Settings& settings) {
    return make_unique<LruCache>(move(booksUnpacker), settings);
}
