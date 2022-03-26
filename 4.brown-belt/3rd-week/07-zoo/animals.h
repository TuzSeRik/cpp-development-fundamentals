#include <string>
// Animals, provided to zoo
class Animal {
public:
    [[nodiscard]] virtual std::string Voice() const {
        return "Not implemented yet";
    }
    virtual ~Animal() = default;
};

class Tiger: public Animal {
    [[nodiscard]] std::string Voice() const override {
        return "Rrrr";
    }
};

class Wolf: public Animal {
    [[nodiscard]] std::string Voice() const override {
        return "Wooo";
    }
};

class Fox: public Animal {
    [[nodiscard]] std::string Voice() const override {
        return "Tyaf";
    }
};
