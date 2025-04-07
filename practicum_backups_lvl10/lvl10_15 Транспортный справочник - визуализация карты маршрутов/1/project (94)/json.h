#pragma once

#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>
#include <stdexcept>
#include <optional>

namespace json {

class Node;
using Dict = std::map<std::string, Node>;

class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Array {
public:
    Array() = default;

    Array(std::initializer_list<Node> nodes) : nodes_(nodes) {}

    // Метод для резервирования памяти
    void reserve(size_t new_capacity) {
        nodes_.reserve(new_capacity);
    }

    // Метод для добавления элемента в конец
    template <typename... Args>
    void emplace_back(Args&&... args) {
        nodes_.emplace_back(std::forward<Args>(args)...);
    }
    
    void push_back(const Node& node) {
        nodes_.push_back(node);
    }

    size_t size() const {
        return nodes_.size();
    }

    const Node& at(size_t index) const {
        return nodes_.at(index);
    }

    bool operator==(const Array& other) const {
        return nodes_ == other.nodes_;
    }

    const Node& operator[](size_t index) const {
        return nodes_.at(index);  // Используйте at для безопасного доступа
    }

    // Итераторы для работы с диапазоном
    auto begin() { return nodes_.begin(); }
    auto end() { return nodes_.end(); }

    // Константные итераторы
    auto begin() const { return nodes_.begin(); }
    auto end() const { return nodes_.end(); }
private:
    std::vector<Node> nodes_;  // Вектор для хранения узлов
};

class Node {
public:
    using Value = std::optional<std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>>;
    Value value_;  // Объявление поля value_

    Node() : value_(nullptr) {}
    Node(Dict map) : value_(map) {}
    Node(int value) : value_(value) {}
    Node(double value) : value_(value) {}
    Node(bool value) : value_(value) {}
    Node(const std::string& value) : value_(value) {}
    Node(const char* value) : value_(std::string(value)) {}
    Node(std::nullptr_t) : value_(nullptr) {}  // Здесь value_ содержит nullptr
    Node(const Array& array) : value_(array) {}

    bool IsNull() const {
        return value_.has_value() && std::holds_alternative<std::nullptr_t>(*value_);
    }

    bool IsInt() const { return value_ && std::holds_alternative<int>(*value_); }
    bool IsDouble() const { return IsInt() || IsPureDouble(); }
    bool IsPureDouble() const { return value_ && std::holds_alternative<double>(*value_); }
    bool IsBool() const { return value_ && std::holds_alternative<bool>(*value_); }
    bool IsString() const { return value_ && std::holds_alternative<std::string>(*value_); }
    bool IsArray() const { return value_ && std::holds_alternative<Array>(*value_); }
    bool IsMap() const { return value_ && std::holds_alternative<Dict>(*value_); }

    int AsInt() const {
        if (IsInt()) return std::get<int>(*value_);
        throw std::logic_error("Not an int");
    }

    bool AsBool() const {
        if (IsBool()) return std::get<bool>(*value_);
        throw std::logic_error("Not a bool");
    }

    double AsDouble() const {
        if (!value_.has_value()) {
            throw std::logic_error("Node is empty");
        }
        if (IsPureDouble()) {
            return std::get<double>(*value_);
        }
        if (IsInt()) {
            return static_cast<double>(std::get<int>(*value_));
        }
        throw std::logic_error("Not a double");
    }

    const std::string& AsString() const {
        if (IsString()) return std::get<std::string>(*value_);
        throw std::logic_error("Not a string");
    }

    const Array& AsArray() const {
        if (IsArray()) return std::get<Array>(*value_);
        throw std::logic_error("Not an array");
    }

    const Dict& AsMap() const {
        if (IsMap()) return std::get<Dict>(*value_);
        throw std::logic_error("Not a map");
    }

    const Value& GetValue() const { return value_; }

    bool operator==(const Node& other) const {
        return value_ == other.value_;
    }

    bool operator!=(const Node& other) const {
        return !(*this == other);
    }
};

class Document {
public:
    // Конструктор с инициализацией корня
    explicit Document(Node root) : root_(std::move(root)) {}

    // Конструктор по умолчанию
    Document() : root_(Node()) {} // Создаем пустой Node или можно использовать nullptr

    // Метод для получения корня
    const Node& GetRoot() const { return root_; }

    // Метод для установки нового корня
    void SetRoot(Node newRoot) { root_ = std::move(newRoot); }

    // Оператор сравнения ==
    bool operator==(const Document& other) const {
        return this->GetRoot() == other.GetRoot(); // Сравниваем корни документов
    }

private:
    Node root_;
};


Document LoadJSON(const std::string& json);
Document Load(std::istream& input);

void Print(const Node& node, std::ostream& output, int indent);
}  // namespace json
