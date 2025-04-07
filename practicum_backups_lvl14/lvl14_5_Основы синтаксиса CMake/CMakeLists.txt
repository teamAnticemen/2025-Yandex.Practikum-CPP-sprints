cmake_minimum_required(VERSION 3.11)

project(ImageProcessing CXX)
set(CMAKE_CXX_STANDARD 17)

set(IMGLIB_FILES 
    img_lib.h img_lib.cpp
    ppm_image.h ppm_image.cpp
)

if(CMAKE_SYSTEM_NAME MATCHES "^MINGW")
    set(SYSTEM_LIBS -lstdc++fs)
else()
    set(SYSTEM_LIBS)
endif()

# Горизонтальное отражение
add_executable(hmirror hmirr.cpp ${IMGLIB_FILES})
target_link_libraries(hmirror PRIVATE ${SYSTEM_LIBS})

# Вертикальное отражение
add_executable(vmirror vmirr.cpp ${IMGLIB_FILES})
target_link_libraries(vmirror PRIVATE ${SYSTEM_LIBS})

# Оператор Собеля
add_executable(sobel sobel.cpp ${IMGLIB_FILES})
target_link_libraries(sobel PRIVATE ${SYSTEM_LIBS})