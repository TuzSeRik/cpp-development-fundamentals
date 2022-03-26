#include "Common.h"

using namespace std;
// Our implementation of IShape
class Figure : public IShape {
public:
    void SetPosition(Point point) override {
        position = point;
    }

    [[nodiscard]] Point GetPosition() const override {
        return position;
    }

    void SetSize(Size size) override {
        this->size = size;
    }

    [[nodiscard]] Size GetSize() const override {
        return size;
    }

    void SetTexture(std::shared_ptr<ITexture> texture) override {
        this->texture = texture;
    }

    [[nodiscard]] ITexture* GetTexture() const override {
        return texture.get();
    }

    void Draw(Image& image) const override {
        if (image.empty())
            return;

        const Image texture_image = texture ? texture->GetImage() :
                                    Image(size.height, std::string(size.width, '.'));

        const int n = image.size();
        const int m = image[0].size();

        for (int i = position.y; i < position.y + size.height; ++i)
            for (int j = position.x; j < position.x + size.width; ++j)
                if (i >= 0 && i < n && j >= 0 && j < m) {
                    const Point point = Point{j - position.x, i - position.y};

                    if (CheckPointInShape(point))
                        if (CheckPointInTexture(point, texture_image))
                            image[i][j] = texture_image[point.y][point.x];
                        else
                            image[i][j] = '.';
                }
    }

private:
    Size size{};
    Point position{};
    shared_ptr<ITexture> texture;

    [[nodiscard]] static bool CheckPointInTexture(Point point, const Image& texture_image) {
        return point.y < texture_image.size() && point.x < texture_image[point.y].size();
    }

    [[nodiscard]] virtual bool CheckPointInShape(Point) const = 0;
};

class Rectangle : public Figure {
public:
    [[nodiscard]] unique_ptr<IShape> Clone() const override {
        return make_unique<Rectangle>(*this);
    }

private:
    [[nodiscard]] bool CheckPointInShape(Point point) const override {
        return true;
    }
};

class Ellipse : public Figure {
public:
    [[nodiscard]] unique_ptr<IShape> Clone() const override {
        return make_unique<Ellipse>(*this);
    }

private:
    [[nodiscard]] bool CheckPointInShape(Point point) const override {
        return IsPointInEllipse(point, GetSize());
    }
};

unique_ptr<IShape> MakeShape(ShapeType shape_type) {
    if (shape_type == ShapeType::Rectangle)
        return make_unique<Rectangle>();
    else
        return make_unique<Ellipse>();
}
