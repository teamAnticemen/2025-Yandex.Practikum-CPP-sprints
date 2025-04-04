#include "bimap.h"
#include <unordered_map>
#include <deque>
#include <stdexcept>
#include <string>

struct BiMap::Impl {
    std::unordered_map<std::string_view, std::string_view> key_to_value;
    std::unordered_map<std::string_view, std::string_view> value_to_key;
    std::deque<std::string> storage; // ��� �������� �����

    Impl() = default;

    // ����������� �����������
    Impl(const Impl& other) {
        storage = other.storage; // �������� ������
        key_to_value = other.key_to_value; // �������� ����
        value_to_key = other.value_to_key; // �������� ����
    }
};

BiMap::BiMap() : pImpl(std::make_unique<Impl>()) {}

BiMap::BiMap(const BiMap& other) : pImpl(std::make_unique<Impl>(*other.pImpl)) {}

BiMap::BiMap(BiMap&& other) noexcept : pImpl(std::move(other.pImpl)) {
    other.pImpl = nullptr; // ��������� �� ������ ���������
}

BiMap& BiMap::operator=(const BiMap& other) {
    if (this == &other) return *this; // �������� �� ����������������
    pImpl = std::make_unique<Impl>(*other.pImpl); // ��������
    return *this;
}

BiMap& BiMap::operator=(BiMap&& other) noexcept {
    if (this == &other) return *this; // �������� �� ����������������
    pImpl = std::move(other.pImpl); // ����������
    other.pImpl = nullptr; // ��������� �� ������ ���������
    return *this;
}

BiMap::~BiMap() = default;

bool BiMap::Add(std::string_view key, std::string_view value) {
    if (pImpl->key_to_value.count(key) || pImpl->value_to_key.count(value)) {
        return false; // ���� ��� �������� ��� ����������
    }

    try {
        // ��������� ������ � deque
        pImpl->storage.emplace_back(key);
        std::string_view key_sv = pImpl->storage.back();
        pImpl->storage.emplace_back(value);
        std::string_view value_sv = pImpl->storage.back();

        // ��������� � �����
        pImpl->key_to_value[key_sv] = value_sv;
        pImpl->value_to_key[value_sv] = key_sv;
    } catch (...) {
        // ����� ��������� � ������ ����������
        pImpl->storage.pop_back(); // ������� value
        pImpl->storage.pop_back(); // ������� key
        throw; // ��������������� ����������
    }

    return true;
}

std::optional<std::string_view> BiMap::FindValue(std::string_view key) const noexcept {
    auto it = pImpl->key_to_value.find(key);
    if (it != pImpl->key_to_value.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<std::string_view> BiMap::FindKey(std::string_view value) const noexcept {
    auto it = pImpl->value_to_key.find(value);
    if (it != pImpl->value_to_key.end()) {
        return it->second;
    }
    return std::nullopt;
}