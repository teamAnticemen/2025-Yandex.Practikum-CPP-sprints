#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

namespace svg {

// Forward declarations
class Object;
class ObjectContainer;

struct Point {
    Point() = default;
    Point(double x, double y) : x(x), y(y) {}
    double x = 0;
    double y = 0;
};

struct RenderContext {
    RenderContext(std::ostream& out, int indent_step = 0, int indent = 0)
        : out(out), indent_step(indent_step), indent(indent) {}

    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};

class Object {
public:
    virtual ~Object() = default;
    void Render(const RenderContext& context) const;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

class Circle final : public Object {
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);
private:
    void RenderObject(const RenderContext& context) const override;

    Point center_ = {0.0, 0.0};
    double radius_ = 1.0;
};

class Polyline final : public Object {
public:
    Polyline& AddPoint(Point point);
private:
    void RenderObject(const RenderContext& context) const override;

    std::vector<Point> points_;
};

class Text final : public Object {
public:
    Text& SetPosition(Point pos);
    Text& SetOffset(Point offset);
    Text& SetFontSize(uint32_t size);
    Text& SetFontFamily(std::string font_family);
    Text& SetFontWeight(std::string font_weight);
    Text& SetData(std::string data);
private:
    void RenderObject(const RenderContext& context) const override;

    Point position_ = {0.0, 0.0};
    Point offset_ = {0.0, 0.0};
    uint32_t font_size_ = 1;
    std::string font_family_;
    std::string font_weight_;
    std::string data_;
};

class Drawable {
public:
    virtual ~Drawable() = default;
    virtual void Draw(ObjectContainer& container) const = 0;
};

class ObjectContainer {
public:
    virtual ~ObjectContainer() = default;

    template <typename Obj>
    void Add(Obj obj) {
        AddPtr(std::make_unique<Obj>(std::move(obj)));
    }

    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
};

class Document : public ObjectContainer {
public:
    void AddPtr(std::unique_ptr<Object>&& obj) override {
        objects_.push_back(std::move(obj));
    }

    void Render(std::ostream& out) const;

private:
    std::vector<std::unique_ptr<Object>> objects_;
};

} // namespace svg