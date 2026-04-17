# Caloryx

`Caloryx` is a course project in C++ for binary storage of users, categories, and food products with restored domain modules such as `Admin`, `User`, `Food`, `Category`, `Meal`, `NutritionPlan`, `Report`, and `Settings`.

## What Was Implemented

The project was rebuilt to match the backend checklist requirements:
- formalized subject area
- plain data structures without methods
- binary storage layer
- separated `Storage`, `Service`, and `UI`
- validation and error handling
- working registration and authorization flow

## Subject Area

Entities used in the system:

- `AccountData`
  - `id`
  - `login`
  - `passwordHash`
  - `role`
  - `displayName`
  - `age`
  - `weight`
  - `height`
  - `dailyCalories`
  - `isDeleted`
- `CategoryData`
  - `id`
  - `name`
  - `isDeleted`
- `FoodData`
  - `id`
  - `name`
  - `caloriesPer100g`
  - `proteinsPer100g`
  - `fatsPer100g`
  - `carbsPer100g`
  - `categoryId`
  - `isDeleted`

## Architecture

The backend is split into four layers:

1. `Models`
   plain data structs only
2. `Storage`
   binary file work, no `cin/cout`
3. `Services`
   validation and business logic
4. `UI`
   console interaction in `main.cpp`

On top of the backend layers, the project also contains restored domain-facing modules:
- `Admin`
- `User`
- `Food`
- `Category`
- `Meal`
- `NutritionPlan`
- `Report`
- `Settings`
- `FoodDatabase`

## Current Structure

```text
src/
  Common/
    FixedText.h
  Admin/
    admin.h/.cpp
  Category/
    category.h/.cpp
  Food/
    food.h/.cpp
  FoodsDatabase/
    FoodsDatabase.h/.cpp
  Models/
    AccountData.h
    AccountRole.h
    CategoryData.h
    FoodData.h
  Meal/
    meal.h/.cpp
  Plan/
    plan.h/.cpp
  Report/
    report.h/.cpp
  Settings/
    settings.h/.cpp
  Storage/
    AccountStorage.h/.cpp
    CategoryStorage.h/.cpp
    FoodStorage.h/.cpp
  Services/
    AuthService.h/.cpp
    CatalogService.h/.cpp
  User/
    user.h/.cpp
  main.cpp

docs/
  backend_checklist.md
  use_cases.md
  manual_testing.md
```

## Binary Files

The program uses these binary files:
- `data/accounts.dat`
- `data/categories.dat`
- `data/foods.dat`

If they do not exist, the application creates them automatically and seeds demo data.

## Implemented Use Cases

- user registration
- user login
- viewing account data
- viewing categories
- viewing foods
- admin category creation
- admin food creation
- admin food update
- admin food deletion
- admin category deletion

## Validation

Implemented validation rules:
- duplicate login is blocked
- duplicate IDs are blocked
- negative nutrition values are blocked
- invalid category reference is blocked
- wrong password is blocked
- missing records do not crash the program
- deleted records are hidden using soft delete

## Restored Project Modules

The original project-oriented modules were restored and adapted to the new backend:
- `Food` and `Category` wrap plain data models
- `Meal`, `NutritionPlan`, and `Report` provide domain calculations
- `User` and `Admin` represent authenticated roles
- `FoodDatabase` works as a domain facade over `CatalogService`

This way the project keeps the original course-project shape while still satisfying the backend checklist.

Important note:
- checklist rules about "data structures without methods" are fulfilled by the files in `src/Models`
- restored domain classes are readable wrappers and facades built above the strict backend layer

## Build And Run

### `g++`

```powershell
cd C:\Users\Dima\Desktop\Kursa
g++ -std=c++17 -O2 -Wall -Wextra -pedantic src\main.cpp src\Admin\admin.cpp src\Category\category.cpp src\Food\food.cpp src\FoodsDatabase\FoodsDatabase.cpp src\Meal\meal.cpp src\Plan\plan.cpp src\Report\report.cpp src\Settings\settings.cpp src\User\user.cpp src\Services\AuthService.cpp src\Services\CatalogService.cpp src\Storage\AccountStorage.cpp src\Storage\CategoryStorage.cpp src\Storage\FoodStorage.cpp -o caloryx.exe
.\caloryx.exe
```

### `CMake`

```powershell
cmake -S . -B build
cmake --build build
.\build\caloryx.exe
```

## Seed Accounts

The first run creates demo accounts:
- `admin / admin123`
- `demo / demo123`

## Documentation

Additional documentation:
- [backend_checklist.md](/C:/Users/Dima/Desktop/Kursa/docs/backend_checklist.md)
- [use_cases.md](/C:/Users/Dima/Desktop/Kursa/docs/use_cases.md)
- [manual_testing.md](/C:/Users/Dima/Desktop/Kursa/docs/manual_testing.md)
