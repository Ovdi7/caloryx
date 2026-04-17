# Backend Checklist Alignment

## 1. Subject Area

Entities:
- `AccountData`: `id`, `login`, `passwordHash`, `role`, `displayName`, `age`, `weight`, `height`, `dailyCalories`, `isDeleted`
- `CategoryData`: `id`, `name`, `isDeleted`
- `FoodData`: `id`, `name`, `caloriesPer100g`, `proteinsPer100g`, `fatsPer100g`, `carbsPer100g`, `categoryId`, `isDeleted`

## 2. Data Modeling

Plain-data headers:
- [AccountData.h](C:/Users/Dima/Desktop/Kursa/src/Models/AccountData.h)
- [CategoryData.h](C:/Users/Dima/Desktop/Kursa/src/Models/CategoryData.h)
- [FoodData.h](C:/Users/Dima/Desktop/Kursa/src/Models/FoodData.h)

Rules applied:
- no methods inside the data structs
- fixed-size `char[]` fields instead of `string`
- fixed primitive types only

Clarification:
- strict checklist compliance is implemented in `src/Models`
- restored classes in folders like `Food`, `Category`, `User`, `Admin`, `Meal`, `Plan`, `Report`, `Settings`, `FoodsDatabase` are domain wrappers above the backend layer and do not replace the required plain-data structs

## 3. Persistence / Storage

Binary storage classes:
- [AccountStorage.h](C:/Users/Dima/Desktop/Kursa/src/Storage/AccountStorage.h)
- [CategoryStorage.h](C:/Users/Dima/Desktop/Kursa/src/Storage/CategoryStorage.h)
- [FoodStorage.h](C:/Users/Dima/Desktop/Kursa/src/Storage/FoodStorage.h)

Storage responsibilities:
- add record
- read all records
- search by identifier
- update record
- soft-delete record

Files:
- `data/accounts.dat`
- `data/categories.dat`
- `data/foods.dat`

## 4. Use Cases

### Register user
- goal: create an account with profile data
- input: login, password, display name, age, weight, height, calorie goal
- result: new stored account

### Login
- goal: authenticate user
- input: login, password
- result: authenticated account

### View account data
- goal: display current account profile
- input: authenticated account id
- result: account data returned from service

### Manage foods
- goal: add, edit, delete, list foods and categories
- input: food/category data
- result: changed binary storage state

## 5. Business Logic

Service classes:
- [AuthService.h](C:/Users/Dima/Desktop/Kursa/src/Services/AuthService.h)
- [CatalogService.h](C:/Users/Dima/Desktop/Kursa/src/Services/CatalogService.h)

Validation:
- duplicate login and duplicate IDs are blocked
- negative nutrition values are blocked
- missing linked category is blocked
- invalid password is blocked

## 6. Working Minimum

Implemented full scenario:
1. registration
2. authorization
3. account data view
4. binary persistence

## 7. Presentation Layer

Console UI is located in:
- [main.cpp](C:/Users/Dima/Desktop/Kursa/src/main.cpp)

UI only:
- reads input
- prints results
- calls services

## 8. Validation & Error Handling

Handled cases:
- missing records
- duplicate login
- duplicate IDs
- invalid credentials
- corrupted or missing storage files

## 9. Manual Testing

Recommended checks:
- empty storage initialization
- duplicate registration
- wrong password
- removing missing record
- adding food with invalid category
