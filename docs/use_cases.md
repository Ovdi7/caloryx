# Use Cases And Function Design

## UC-1 Register User

Goal:
- create a new user account and save it in binary storage

Input:
- `login`
- `password`
- `displayName`
- `age`
- `weight`
- `height`
- `dailyCalories`

Expected result:
- new `AccountData` record is stored

Main function:
- `AuthService::registerUser(login, password, displayName, age, weight, height, dailyCalories)`

Algorithm:
1. validate required fields
2. validate positive numeric values
3. check duplicate login through `AccountStorage`
4. hash password
5. create `AccountData`
6. save record in binary file
7. return created account

## UC-2 Login

Goal:
- authenticate an existing account

Input:
- `login`
- `password`

Expected result:
- authenticated `AccountData`

Main function:
- `AuthService::login(login, password)`

Algorithm:
1. find account by login
2. hash entered password
3. compare with stored hash
4. return account on success
5. throw validation error on failure

## UC-3 View Account

Goal:
- get account information after authorization

Input:
- `accountId`

Expected result:
- account data for viewing in UI

Main function:
- `AuthService::getAccountById(id)`

Algorithm:
1. find account by id in storage
2. return found account
3. throw error if account does not exist

## UC-4 Add Category

Goal:
- create a new food category

Input:
- `id`
- `name`

Expected result:
- new category stored in binary file

Main function:
- `CatalogService::addCategory(id, name)`

Algorithm:
1. validate category name
2. check duplicate id
3. create `CategoryData`
4. save through `CategoryStorage`

## UC-5 Add Food

Goal:
- create a new product in catalog

Input:
- `id`
- `name`
- `caloriesPer100g`
- `proteinsPer100g`
- `fatsPer100g`
- `carbsPer100g`
- `categoryId`

Expected result:
- new food stored in binary file

Main function:
- `CatalogService::addFood(...)`

Algorithm:
1. validate name and nutrition values
2. check duplicate food id
3. validate category existence
4. create `FoodData`
5. save through `FoodStorage`

## UC-6 Update Food

Goal:
- edit existing food record

Input:
- `id`
- new field values

Expected result:
- modified food data in binary file

Main function:
- `CatalogService::updateFood(...)`

Algorithm:
1. validate category existence
2. find existing food by id
3. replace editable fields
4. save updated record

## UC-7 Delete Food / Category

Goal:
- remove record from active use without damaging binary file

Input:
- `id`

Expected result:
- record marked as deleted

Main functions:
- `CatalogService::removeFood(id)`
- `CatalogService::removeCategory(id)`

Algorithm:
1. find record by id
2. for category deletion ensure no food depends on it
3. mark `isDeleted = 1`
4. rewrite binary file
