#include "ini.h"

namespace Ini {
    // Some simple document methods
    Section& Document::AddSection(const string& name) {
        sections.insert({name, Section()});
        return sections.at(name);
    }

    const Section& Document::GetSection(const string &name) const {
        return sections.at(name);
    }

    size_t Document::SectionCount() const {
        return sections.size();
    }
    // Main document method
    Document Load(istream& input) {
        Document result;
        string currentSectionName;
        Section currentSection;
        string line;
        // Using >> operator, because I kinda hate getline
        while (input >> line) {
            // New section
            if (line.at(0) == '[') {
                // Saving already parsed section, if header not empty
                if (!currentSectionName.empty()) {
                    result.AddSection(currentSectionName) = currentSection;
                    currentSection.clear();
                }
                // Parsing other parts of multi-word header
                while (*line.rbegin() != ']') {
                    string tmp;
                    input >> tmp;
                    line.append(" " + tmp);
                }
                // Saving new section name
                currentSectionName = {begin(line) + line.find_first_not_of('['),
                                      begin(line) + line.find_first_of(']')};
            }
            else
                // Saving section elements to temporary section
                currentSection.insert({{begin(line), begin(line) + line.find('=')},
                                       {begin(line) + line.find('=') + 1, end(line)}});
        }
        if (!currentSectionName.empty())
            // Last saved section adding
            result.AddSection(currentSectionName) = currentSection;

        return result;
    }
}
