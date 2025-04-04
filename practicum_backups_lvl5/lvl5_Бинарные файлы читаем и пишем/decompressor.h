#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdint>

class DecompressorRLE {
public:
    inline bool DecodeRLE(const std::string& src_name, const std::string& dst_name);
};

// напишите эту функцию
inline bool DecodeRLE(const std::string& src_name, const std::string& dst_name)
{
    std::ifstream inFile(src_name, std::ios::binary);  

    if (!inFile.is_open()) {
        std::cerr << "Ошибка открытия файлов!" << std::endl;
        return false;
    }
    
    std::ofstream outFile(dst_name, std::ios::binary);//открыть поток только после проверки входного файла
    
    while (true) {
        uint8_t header;
        inFile.read(reinterpret_cast<char*>(&header), sizeof(header));
        if (inFile.eof()) break; // Конец файла

        bool isCompressed = header & 0x01;  // Получаем тип блока
        int size = (header >> 1) + 1;       // Получаем размер данных

        if (isCompressed) { // Блок-серия
            uint8_t dataByte;
            inFile.read(reinterpret_cast<char*>(&dataByte), sizeof(dataByte));
            for (int i = 0; i < size; ++i) {
                outFile.put(dataByte); // Записываем байт size раз
            }
        } else { // Блок без сжатия
            std::vector<uint8_t> data(size);
            inFile.read(reinterpret_cast<char*>(data.data()), size);
            outFile.write(reinterpret_cast<const char*>(data.data()), size);
        }
    }

    inFile.close();
    outFile.close();
    return true;//File opened
}