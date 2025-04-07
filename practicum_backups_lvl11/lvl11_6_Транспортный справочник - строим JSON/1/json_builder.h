#pragma once

#include "json.h"

#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

namespace json {

class Builder {
public:
    Builder() = default;

    // Методы для построения JSON
    Builder& Value(Node::Value value);
    Builder& StartDict();
    Builder& StartArray();
    Builder& Key(std::string key);
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
};

}  // namespace json