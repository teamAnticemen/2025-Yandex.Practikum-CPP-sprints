#include "json_builder.h"

namespace json {

DictItemContext Builder::StartDict() {
    EnsureNotBuilt();
    EnsureInArrayOrDictContext();

    if (is_empty_) {
        is_empty_ = false;
    }

    Node node(Dict{});
    if (nodes_stack_.empty()) {
        root_ = node;
        nodes_stack_.push_back(&root_);
    } else if (nodes_stack_.back()->IsArray()) {
        const_cast<Array&>(nodes_stack_.back()->AsArray()).push_back(node);
        nodes_stack_.push_back(&const_cast<Array&>(nodes_stack_.back()->AsArray()).back());
    } else if (nodes_stack_.back()->IsDict()) {
        if (current_key_.empty() && !key_is_entered_) {
            Reset();
            throw std::logic_error("StartDict() called after Key() without a value");
        }
        const_cast<Dict&>(nodes_stack_.back()->AsDict())[current_key_] = node;
        nodes_stack_.push_back(&const_cast<Dict&>(nodes_stack_.back()->AsDict())[current_key_]);
        current_key_.clear();
        key_is_entered_ = false;
    }

    open_dicts_++;
    return DictItemContext(*this);
}

ArrayItemContext Builder::StartArray() {
    EnsureNotBuilt();
    EnsureInArrayOrDictContext();

    if (is_empty_) {
        is_empty_ = false;
    }

    Node node(Array{});
    if (nodes_stack_.empty()) {
        root_ = node;
        nodes_stack_.push_back(&root_);
    } else if (nodes_stack_.back()->IsArray()) {
        const_cast<Array&>(nodes_stack_.back()->AsArray()).push_back(node);
        nodes_stack_.push_back(&const_cast<Array&>(nodes_stack_.back()->AsArray()).back());
    } else if (nodes_stack_.back()->IsDict()) {
        if (current_key_.empty() && !key_is_entered_) {
            Reset();
            throw std::logic_error("StartArray() called after Key() without a value");
        }
        const_cast<Dict&>(nodes_stack_.back()->AsDict())[current_key_] = node;
        nodes_stack_.push_back(&const_cast<Dict&>(nodes_stack_.back()->AsDict())[current_key_]);
        current_key_.clear();
        key_is_entered_ = false;
    }

    open_arrays_++;
    return ArrayItemContext(*this);
}

KeyContext Builder::Key(std::string key) {
    EnsureNotBuilt();
    EnsureInDictContext();

    if (!current_key_.empty()) {
        Reset();
        throw std::logic_error("Key() called after Key() without a value");
    }

    current_key_ = key;
    key_is_entered_ = true;
    return KeyContext(*this);
}

Builder& Builder::Value(Node::Value value) {
    EnsureNotBuilt();
    EnsureInArrayOrDictContext();

    if (is_empty_) {
        is_empty_ = false;
    }

    Node node = std::visit([](auto&& arg) { return Node(arg); }, value);

    if (nodes_stack_.empty()) {
        root_ = node;
        is_built_ = true;
        key_is_entered_ = true;
    } else if (nodes_stack_.back()->IsArray()) {
        const_cast<Array&>(nodes_stack_.back()->AsArray()).push_back(node);
    } else if (nodes_stack_.back()->IsDict()) {
        if (current_key_.empty() && !key_is_entered_) {
            Reset();
            throw std::logic_error("Value() called after Key() without a value");
        }
        const_cast<Dict&>(nodes_stack_.back()->AsDict())[current_key_] = node;
        current_key_.clear();
        key_is_entered_ = false;
    }

    return *this;
}

Builder& Builder::EndDict() {
    EnsureNotBuilt();
    EnsureInDictContext();

    nodes_stack_.pop_back();
    open_dicts_--;
    return *this;
}

Builder& Builder::EndArray() {
    EnsureNotBuilt();
    EnsureInArrayContext();

    nodes_stack_.pop_back();
    open_arrays_--;
    return *this;
}

Node Builder::Build() {

    if (is_empty_) {
        Reset();
        throw std::logic_error("Build() called on empty builder");
    }

    if (nodes_stack_.empty() && is_built_) {
        Node result = root_;
        Reset();
        return result;
    }

    if (open_dicts_ != 0 || open_arrays_ != 0) {
        Reset();
        throw std::logic_error("Build() called with unfinished containers");
    }

    if (nodes_stack_.empty()) {
        is_built_ = true;
        Node result = root_;
        Reset();
        return result;
    }

    Reset();
    throw std::logic_error("Build() called on empty builder");
}

void Builder::Reset() {
    nodes_stack_.clear();
    root_ = Node();
    is_built_ = false;
    current_key_.clear();
    key_is_entered_ = false;
    open_dicts_ = 0;
    open_arrays_ = 0;
    is_empty_ = true;
}

void Builder::EnsureNotBuilt() {
    if (is_built_) {
        Reset();
        throw std::logic_error("Builder is already built");
    }
}

void Builder::EnsureInDictContext() {
    if (nodes_stack_.empty() || !nodes_stack_.back()->IsDict()) {
        Reset();
        throw std::logic_error("Key() called outside of a dictionary");
    }
}

void Builder::EnsureInArrayOrDictContext() {
    if (!nodes_stack_.empty() && !nodes_stack_.back()->IsArray() && !nodes_stack_.back()->IsDict()) {
        Reset();
        throw std::logic_error("Value() called in invalid context");
    }
}

void Builder::EnsureInArrayContext() {
    if (nodes_stack_.empty() || !nodes_stack_.back()->IsArray()) {
        Reset();
        throw std::logic_error("EndArray() called outside of an array");
    }
}

// Реализация методов вспомогательных классов

KeyContext DictItemContext::Key(std::string key) {
    return builder_.Key(std::move(key));
}

Builder& DictItemContext::EndDict() {
    return builder_.EndDict();
}

ArrayItemContext ArrayItemContext::Value(Node::Value value) {
    builder_.Value(std::move(value));
    return *this;
}

DictItemContext ArrayItemContext::StartDict() {
    return builder_.StartDict();
}

ArrayItemContext ArrayItemContext::StartArray() {
    return builder_.StartArray();
}

Builder& ArrayItemContext::EndArray() {
    return builder_.EndArray();
}

DictItemContext KeyContext::Value(Node::Value value) {
    builder_.Value(std::move(value));
    return DictItemContext(builder_);
}

DictItemContext KeyContext::StartDict() {
    return builder_.StartDict();
}

ArrayItemContext KeyContext::StartArray() {
    return builder_.StartArray();
}

}  // namespace json