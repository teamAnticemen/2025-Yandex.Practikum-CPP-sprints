#pragma once

#include "json.h"

#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

namespace json {

class Builder;

class DictItemContext;
class ArrayItemContext;
class KeyContext;

class Builder {
public:
    Builder() = default;

    // Методы для построения JSON
    DictItemContext StartDict();
    ArrayItemContext StartArray();
    KeyContext Key(std::string key);
    Builder& Value(Node::Value value);
    Builder& EndDict();
    Builder& EndArray();

    // Завершает построение JSON и возвращает результат
    Node Build();

private:
    // Вспомогательные методы и структуры
    void EnsureNotBuilt();
    void EnsureInDictContext();
    void EnsureInArrayOrDictContext();
    void EnsureInArrayContext();
    void Reset();

    // Стек для отслеживания текущего контекста
    std::vector<Node*> nodes_stack_;
    Node root_;
    bool is_built_ = false;
    std::string current_key_;  // Текущий ключ для словаря
    bool key_is_entered_ = false;

    // Счетчики для отслеживания открытых контейнеров
    int open_dicts_ = 0;
    int open_arrays_ = 0;

    // Флаг для отслеживания пустого Builder
    bool is_empty_ = true;

    friend class DictItemContext;
    friend class ArrayItemContext;
    friend class KeyContext;
};

class DictItemContext {
public:
    DictItemContext(Builder& builder) : builder_(builder) {}

    KeyContext Key(std::string key);
    Builder& EndDict();

private:
    Builder& builder_;
};

class ArrayItemContext {
public:
    ArrayItemContext(Builder& builder) : builder_(builder) {}

    ArrayItemContext Value(Node::Value value);
    DictItemContext StartDict();
    ArrayItemContext StartArray();
    Builder& EndArray();

private:
    Builder& builder_;
};

class KeyContext {
public:
    KeyContext(Builder& builder) : builder_(builder) {}

    DictItemContext Value(Node::Value value);
    DictItemContext StartDict();
    ArrayItemContext StartArray();

private:
    Builder& builder_;
};

}  // namespace json