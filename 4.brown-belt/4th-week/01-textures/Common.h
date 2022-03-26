#pragma once

#include <memory>
#include <string>
#include <vector>
// Base class and some methods for our solution
struct Point {
    int x;
    int y;
};

struct Size {
    int width;
    int height;
};

inline bool IsPointInEllipse(Point p, Size size) {
    double x = (p.x + 0.5) / (size.width / 2.0) - 1;
    double y = (p.y + 0.5) / (size.height / 2.0) - 1;

    return x * x + y * y <= 1;
}

using Image = std::vector<std::string>;

enum class ShapeType {
    Rectangle,
    Ellipse
};

class ITexture {
public:
    virtual ~ITexture() = default;

    [[nodiscard]] virtual Size GetSize() const = 0;

    [[nodiscard]] virtual const Image& GetImage() const = 0;
};

class IShape {
public:
    virtual ~IShape() = default;

    [[nodiscard]] virtual std::unique_ptr<IShape> Clone() const = 0;

    virtual void SetPosition(Point) = 0;
    [[nodiscard]] virtual Point GetPosition() const = 0;

    virtual void SetSize(Size) = 0;
    [[nodiscard]] virtual Size GetSize() const = 0;

    virtual void SetTexture(std::shared_ptr<ITexture>) = 0;
    [[nodiscard]] virtual ITexture* GetTexture() const = 0;

    virtual void Draw(Image&) const = 0;
};

std::unique_ptr<IShape> MakeShape(ShapeType shape_type);
