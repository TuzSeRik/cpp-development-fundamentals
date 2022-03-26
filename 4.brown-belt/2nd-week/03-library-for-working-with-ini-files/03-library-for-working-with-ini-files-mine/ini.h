#pragma once

#include <iostream>
#include <unordered_map>

using namespace std;
// Header of INI parser
namespace Ini {
    using Section = unordered_map<string, string>;

    class Document {
    public:
        // Manipulation with parts of document
        Section& AddSection(const string& name);

        const Section& GetSection(const string &name) const;

        size_t SectionCount() const;

    private:
        unordered_map<string, Section> sections;
    };
    // Loading document from input stream
    Document Load(istream& input);
}
