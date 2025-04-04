#include "ini.h"
#include <sstream>
#include <algorithm>

namespace ini {

Section& Document::AddSection(std::string name) {
    // Добавляем новую секцию и возвращаем ссылку на неё
    return sections_[std::move(name)];
}

const Section& Document::GetSection(const std::string& name) const {
    auto it = sections_.find(name);
    if (it != sections_.end()) {
        return it->second; // Возвращаем ссылку на существующую секцию
    }
    static const Section empty_section; // Статическая пустая секция
    return empty_section; // Возвращаем ссылку на пустую секцию
}

std::size_t Document::GetSectionCount() const {
    return sections_.size();
}

Document Load(std::istream& input) {
    Document doc;
    std::string line;
    std::string current_section;

    while (std::getline(input, line)) {
        // Удаляем пробелы в начале и конце строки
        line.erase(line.find_last_not_of(" \n\r\t") + 1);
        line.erase(0, line.find_first_not_of(" \n\r\t"));

        if (line.empty()) {
            continue; // Игнорируем пустые строки
        }

        if (line.front() == '[' && line.back() == ']') {
            // Это имя секции
            current_section = line.substr(1, line.size() - 2);
            doc.AddSection(current_section); // Добавляем секцию
        } else {
            // Это присваивание
            auto equal_pos = line.find('=');
            if (equal_pos != std::string::npos && !current_section.empty()) {
                std::string key = line.substr(0, equal_pos);
                std::string value = line.substr(equal_pos + 1);

                // Удаляем пробелы вокруг ключа и значения
                key.erase(key.find_last_not_of(" \n\r\t") + 1);
                key.erase(0, key.find_first_not_of(" \n\r\t"));
                value.erase(value.find_last_not_of(" \n\r\t") + 1);
                value.erase(0, value.find_first_not_of(" \n\r\t"));

                // Добавляем ключ-значение в текущую секцию
                doc.AddSection(current_section)[key] = value;
            }
        }
    }

    return doc;
}

} // namespace ini
