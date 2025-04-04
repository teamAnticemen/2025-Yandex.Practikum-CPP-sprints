#include "svg.h"

namespace svg {

std::ostream& operator<<(std::ostream& out, StrokeLineCap line_cap) {
    switch (line_cap) {
        case StrokeLineCap::BUTT: out << "butt"; break;
        case StrokeLineCap::ROUND: out << "round"; break;
        case StrokeLineCap::SQUARE: out << "square"; break;
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, StrokeLineJoin line_join) {
    switch (line_join) {
        case StrokeLineJoin::ARCS: out << "arcs"; break;
        case StrokeLineJoin::BEVEL: out << "bevel"; break;
        case StrokeLineJoin::MITER: out << "miter"; break;
        case StrokeLineJoin::MITER_CLIP: out << "miter-clip"; break;
        case StrokeLineJoin::ROUND: out << "round"; break;
    }
    return out;
}

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();
    RenderObject(context);
    context.out << std::endl;
}

// ---------- PathProps ------------------

PathProps_T& PathProps_T::SetFillColor(Color color) {
    fill_color_ = std::move(color);
    return *this;
}

PathProps_T& PathProps_T::SetStrokeColor(Color color) {
    stroke_color_ = std::move(color);
    return *this;
}

PathProps_T& PathProps_T::SetStrokeWidth(double width) {
    stroke_width_ = width;
    return *this;
}

PathProps_T& PathProps_T::SetStrokeLineCap(StrokeLineCap line_cap) {
    stroke_line_cap_ = line_cap;
    return *this;
}

PathProps_T& PathProps_T::SetStrokeLineJoin(StrokeLineJoin line_join) {
    stroke_line_join_ = line_join;
    return *this;
}

void PathProps_T::RenderAttrs(std::ostream& out) const {
    if (fill_color_) {
        out << " fill=\"" << *fill_color_ << "\"";
    }
    if (stroke_color_) {
        out << " stroke=\"" << *stroke_color_ << "\"";
    }
    if (stroke_width_) {
        out << " stroke-width=\"" << *stroke_width_ << "\"";
    }
    if (stroke_line_cap_) {
        out << " stroke-linecap=\"" << *stroke_line_cap_ << "\"";
    }
    if (stroke_line_join_) {
        out << " stroke-linejoin=\"" << *stroke_line_join_ << "\"";
    }
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center) {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius) {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\"" << center_.x << "\" cy=\"" << center_.y << "\" r=\"" << radius_ << "\"";
    RenderAttrs(out);
    out << " />";
}

// ---------- Polyline ------------------

Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(point);
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\"";
    for (size_t i = 0; i < points_.size(); ++i) {
        if (i > 0) {
            out << " ";
        }
        out << points_[i].x << "," << points_[i].y;
    }
    out << "\"";
    RenderAttrs(out);
    out << " />";
}

// ---------- Text ------------------

Text& Text::SetPosition(Point pos) {
    position_ = pos;
    return *this;
}

Text& Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}

Text& Text::SetFontSize(uint32_t size) {
    font_size_ = size;
    return *this;
}

Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = std::move(font_family);
    return *this;
}

Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = std::move(font_weight);
    return *this;
}

Text& Text::SetData(std::string data) {
    data_ = std::move(data);
    return *this;
}

void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text x=\"" << position_.x << "\" y=\"" << position_.y
        << "\" dx=\"" << offset_.x << "\" dy=\"" << offset_.y
        << "\" font-size=\"" << font_size_ << "\"";

    if (!font_family_.empty()) {
        out << " font-family=\"" << font_family_ << "\"";
    }
    if (!font_weight_.empty()) {
        out << " font-weight=\"" << font_weight_ << "\"";
    }

    RenderAttrs(out);
    out << ">" << data_ << "</text>";
}

// ---------- Document ------------------

void Document::Render(std::ostream& out) const {
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n";
    RenderContext context(out, 2);
    for (const auto& obj : objects_) {
        obj->Render(context);
    }
    out << "</svg>";
}

} // namespace svg