cmake_minimum_required(VERSION 3.16)
project(Caloryx LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(caloryx_backend
    src/Admin/admin.cpp
    src/Calculator/CalorieCalculator.cpp
    src/Category/category.cpp
    src/Food/food.cpp
    src/FoodsDatabase/FoodsDatabase.cpp
    src/Meal/meal.cpp
    src/Plan/plan.cpp
    src/Report/report.cpp
    src/Settings/settings.cpp
    src/Services/AuthService.cpp
    src/Services/CatalogService.cpp
    src/Storage/AccountStorage.cpp
    src/User/user.cpp
)

target_include_directories(caloryx_backend PUBLIC src)

add_executable(caloryx src/main.cpp)
target_link_libraries(caloryx PRIVATE caloryx_backend)
