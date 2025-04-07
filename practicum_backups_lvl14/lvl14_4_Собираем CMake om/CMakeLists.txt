cmake_minimum_required(VERSION 3.10)
project(Negate)

# Устанавливаем стандарт C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем исполняемый файл negate
add_executable(negate
    main.cpp
    img_lib.cpp
    ppm_image.cpp
)

# Для работы с filesystem может потребоваться эта библиотека
target_link_libraries(negate PRIVATE stdc++fs)