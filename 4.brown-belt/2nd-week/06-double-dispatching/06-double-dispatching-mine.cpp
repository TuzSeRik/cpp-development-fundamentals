#include <vector>
#include <memory>

#include "geo2d.h"
#include "game_object.h"
#include "test_runner.h"

using namespace std;
// Awful task - we should write same code for many times because we can't utilize OOP properly and modify base class
// Classes declaration (which we can't event separate to another header)
class Unit : public GameObject {
public:
    explicit Unit(geo2d::Point position) : position(position) {}

    [[nodiscard]] bool Collide(const GameObject& that) const override;

    [[nodiscard]] bool CollideWith(const Unit &that) const override;

    [[nodiscard]] bool CollideWith(const Building& that) const override;

    [[nodiscard]] bool CollideWith(const Tower &that) const override;

    [[nodiscard]] bool CollideWith(const Fence &that) const override;

    const geo2d::Point position;
};

class Building : public GameObject {
public:
    explicit Building(geo2d::Rectangle geometry) : geometry(geometry) {}

    [[nodiscard]] bool Collide(const GameObject& that) const override;

    [[nodiscard]] bool CollideWith(const Unit &that) const override;

    [[nodiscard]] bool CollideWith(const Building &that) const override;

    [[nodiscard]] bool CollideWith(const Tower &that) const override;

    [[nodiscard]] bool CollideWith(const Fence &that) const override;

    const geo2d::Rectangle geometry;
};

class Tower : public GameObject {
public:
    explicit Tower(geo2d::Circle geometry) : geometry(geometry) {}

    [[nodiscard]] bool Collide(const GameObject& that) const override;

    [[nodiscard]] bool CollideWith(const Unit &that) const override;

    [[nodiscard]] bool CollideWith(const Building &that) const override;

    [[nodiscard]] bool CollideWith(const Tower &that) const override;

    [[nodiscard]] bool CollideWith(const Fence &that) const override;

    const geo2d::Circle geometry;
};

class Fence : public GameObject {
public:
    explicit Fence(geo2d::Segment geometry) : geometry(geometry) {}

    [[nodiscard]] bool Collide(const GameObject& that) const override;

    [[nodiscard]] bool CollideWith(const Unit &that) const override;

    [[nodiscard]] bool CollideWith(const Building &that) const override;

    [[nodiscard]] bool CollideWith(const Tower &that) const override;

    [[nodiscard]] bool CollideWith(const Fence &that) const override;

    const geo2d::Segment geometry;
};
// Classes methods definition
// Collide method dynamically dedicate which method CollideWith to use
bool Unit::Collide(const GameObject &that) const {
    return that.CollideWith(*this);
}
// CollideWith method just calls defined Collide method from provided library
bool Unit::CollideWith(const Unit &that) const {
    return geo2d::Collide(this->position, that.position);
}
// ANY CollideWith method do so and we can't even simplify this by defining base class methods
bool Unit::CollideWith(const Building &that) const {
    return geo2d::Collide(this->position, that.geometry);
}

bool Unit::CollideWith(const Tower &that) const {
    return geo2d::Collide(this->position, that.geometry);
}

bool Unit::CollideWith(const Fence &that) const {
    return geo2d::Collide(this->position, that.geometry);
}

bool Building::Collide(const GameObject &that) const {
    return that.CollideWith(*this);
}

bool Building::CollideWith(const Unit &that) const {
    return geo2d::Collide(this->geometry, that.position);
}

bool Building::CollideWith(const Building &that) const {
    return geo2d::Collide(this->geometry, that.geometry);
}

bool Building::CollideWith(const Tower &that) const {
    return geo2d::Collide(this->geometry, that.geometry);
}

bool Building::CollideWith(const Fence &that) const {
    return geo2d::Collide(this->geometry, that.geometry);
}

bool Tower::Collide(const GameObject &that) const {
    return that.CollideWith(*this);
}

bool Tower::CollideWith(const Unit &that) const {
    return geo2d::Collide(this->geometry, that.position);
}

bool Tower::CollideWith(const Building &that) const {
    return geo2d::Collide(this->geometry, that.geometry);
}

bool Tower::CollideWith(const Tower &that) const {
    return geo2d::Collide(this->geometry, that.geometry);
}

bool Tower::CollideWith(const Fence &that) const {
    return geo2d::Collide(this->geometry, that.geometry);
}

bool Fence::Collide(const GameObject &that) const {
    return that.CollideWith(*this);
}

bool Fence::CollideWith(const Unit &that) const {
    return geo2d::Collide(this->geometry, that.position);
}

bool Fence::CollideWith(const Building &that) const {
    return geo2d::Collide(this->geometry, that.geometry);
}

bool Fence::CollideWith(const Tower &that) const {
    return geo2d::Collide(this->geometry, that.geometry);
}

bool Fence::CollideWith(const Fence &that) const {
    return geo2d::Collide(this->geometry, that.geometry);
}
// End of bloat code
// This function calls collide method, which will call method CollideWith,
// which will dedicate what concrete type first object are and call geo2d::Collide for that pair
bool Collide(const GameObject& first, const GameObject& second) {
    return first.Collide(second);
}
// Tests, provided by authors
void TestAddingNewObjectOnMap() {
    using namespace geo2d;

    const vector<shared_ptr<GameObject>> game_map = {
            make_shared<Unit>(Point{3, 3}),
            make_shared<Unit>(Point{5, 5}),
            make_shared<Unit>(Point{3, 7}),
            make_shared<Fence>(Segment{{7, 3}, {9, 8}}),
            make_shared<Tower>(Circle{Point{9, 4}, 1}),
            make_shared<Tower>(Circle{Point{10, 7}, 1}),
            make_shared<Building>(Rectangle{{11, 4}, {14, 6}})
    };

    for (size_t i = 0; i < game_map.size(); ++i) {
        Assert(
                Collide(*game_map[i], *game_map[i]),
                "An object doesn't collide with itself: " + to_string(i)
        );

        for (size_t j = 0; j < i; ++j) {
            Assert(
                    !Collide(*game_map[i], *game_map[j]),
                    "Unexpected collision found " + to_string(i) + ' ' + to_string(j)
            );
        }
    }

    auto new_warehouse = make_shared<Building>(Rectangle{{4, 3}, {9, 6}});
    ASSERT(!Collide(*new_warehouse, *game_map[0]))
    ASSERT( Collide(*new_warehouse, *game_map[1]))
    ASSERT(!Collide(*new_warehouse, *game_map[2]))
    ASSERT( Collide(*new_warehouse, *game_map[3]))
    ASSERT( Collide(*new_warehouse, *game_map[4]))
    ASSERT(!Collide(*new_warehouse, *game_map[5]))
    ASSERT(!Collide(*new_warehouse, *game_map[6]))

    auto new_defense_tower = make_shared<Tower>(Circle{{8, 2}, 2});
    ASSERT(!Collide(*new_defense_tower, *game_map[0]))
    ASSERT(!Collide(*new_defense_tower, *game_map[1]))
    ASSERT(!Collide(*new_defense_tower, *game_map[2]))
    ASSERT( Collide(*new_defense_tower, *game_map[3]))
    ASSERT( Collide(*new_defense_tower, *game_map[4]))
    ASSERT(!Collide(*new_defense_tower, *game_map[5]))
    ASSERT(!Collide(*new_defense_tower, *game_map[6]))
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestAddingNewObjectOnMap);
    return 0;
}
