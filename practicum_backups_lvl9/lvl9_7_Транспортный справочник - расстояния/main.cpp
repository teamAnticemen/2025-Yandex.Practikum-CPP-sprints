#include <chrono>
#include <iostream>
#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"

int main() {
    //auto start_time = std::chrono::high_resolution_clock::now(); // Начало измерения времени
    transport_catalogue::TransportCatalogue catalogue;
    input_reader::InputReader input_reader;

    int n;
    std::cin >> n;
    std::cin.ignore(); // Игнорируем оставшийся символ новой строки
    
    for (int i = 0; i < n; ++i) {
        std::string line;
        std::getline(std::cin, line);
        input_reader.ParseLine(line);
    }
    
    input_reader.ApplyCommands(catalogue);       

    int q;
    std::cin >> q;
    std::cin.ignore(); // Игнорируем оставшийся символ новой строки

    stat_reader::StatReader stat_reader(catalogue);

    for (int i = 0; i < q; ++i) {
        std::string line;
        std::getline(std::cin, line);
        stat_reader.ProcessQuery(line);
    }       

    //stat_reader.PrintAllBuses(); //Вывести весь каталог //Отладка
    /*auto end_time = std::chrono::high_resolution_clock::now();
    std::cout << "Main: " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() 
              << " милисекунд" << std::endl;*/
    
    //catalogue.PrintCollisions(); // Вывод количества коллизий
    return 0;
}