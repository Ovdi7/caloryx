cmake_minimum_required(VERSION 3.16)
project(Caloryx LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSION OFF)

# 1. Збираємо файли проєкту (main.cpp та всі інші логічні класи з папки src)
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(caloryx src/main.cpp ${SOURCES})

# 2. Прописуємо правильні шляхи до папок з бібліотеками (включаючи asio)
target_include_directories(caloryx PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/asio
    ${CMAKE_SOURCE_DIR}/include/asio/include
)

# 3. Підключаємо мережеві бібліотеки Windows (Winsock), щоб не було помилок лінкера
target_link_libraries(caloryx PRIVATE ws2_32 mswsock)