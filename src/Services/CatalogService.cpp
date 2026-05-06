#include "CatalogService.h"

#include <stdexcept>

#include "../Common/FixedText.h"
#include "../Category/category.h"

CatalogService::CatalogService(FoodStorage& foodStorage)
    : foodStorage(foodStorage) {
}

void CatalogService::ensureSeedData() const {
    if (!foodStorage.fileExists()) {
        foodStorage.initializeEmpty();
    }

    if (foodStorage.getAll().empty()) {
        addFood(1, "Apple", 52, 0.3, 0.2, 14.0, 1);
        addFood(2, "Banana", 96, 1.3, 0.3, 27.0, 1);
        addFood(3, "Chicken breast", 165, 31.0, 3.6, 0.0, 2);
        addFood(4, "Rice cooked", 130, 2.7, 0.3, 28.0, 3);
        addFood(5, "Greek yogurt", 59, 10.0, 0.4, 3.6, 4);
        addFood(6, "Almonds", 579, 21.2, 49.9, 21.6, 5);
    }
}

std::vector<FoodData> CatalogService::getAllFoods() const {
    return foodStorage.getAll();
}

FoodData CatalogService::getFoodById(int id) const {
    FoodData food{};
    if (!foodStorage.findById(id, food)) {
        throw std::runtime_error("Food not found.");
    }
    return food;
}

void CatalogService::addFood(
    int id,
    const std::string& name,
    int caloriesPer100g,
    double proteinsPer100g,
    double fatsPer100g,
    double carbsPer100g,
    int categoryId
) const {
    if (name.empty()) {
        throw std::runtime_error("Food name is required.");
    }
    if (name.length() >= FoodData::NameSize) {
        throw std::runtime_error("Food name is too long.");
    }
    if (caloriesPer100g < 0 || proteinsPer100g < 0.0 || fatsPer100g < 0.0 || carbsPer100g < 0.0) {
        throw std::runtime_error("Food nutrition values cannot be negative.");
    }

    FoodData existing{};
    if (foodStorage.findById(id, existing)) {
        throw std::runtime_error("Food ID already exists.");
    }

    if (!isValidCategory(categoryId)) {
        throw std::runtime_error("Category does not exist.");
    }

    FoodData food{};
    food.id = id;
    copyText(food.name, FoodData::NameSize, name);
    food.caloriesPer100g = caloriesPer100g;
    food.proteinsPer100g = proteinsPer100g;
    food.fatsPer100g = fatsPer100g;
    food.carbsPer100g = carbsPer100g;
    food.categoryId = categoryId;
    food.isDeleted = 0;

    foodStorage.add(food);
}

void CatalogService::updateFood(
    int id,
    const std::string& name,
    int caloriesPer100g,
    double proteinsPer100g,
    double fatsPer100g,
    double carbsPer100g,
    int categoryId
) const {
    if (!isValidCategory(categoryId)) {
        throw std::runtime_error("Category does not exist.");
    }

    FoodData food{};
    if (!foodStorage.findById(id, food)) {
        throw std::runtime_error("Food not found.");
    }
    if (name.empty()) {
        throw std::runtime_error("Food name is required.");
    }
    if (name.length() >= FoodData::NameSize) {
        throw std::runtime_error("Food name is too long.");
    }

    copyText(food.name, FoodData::NameSize, name);
    food.caloriesPer100g = caloriesPer100g;
    food.proteinsPer100g = proteinsPer100g;
    food.fatsPer100g = fatsPer100g;
    food.carbsPer100g = carbsPer100g;
    food.categoryId = categoryId;

    if (!foodStorage.update(food)) {
        throw std::runtime_error("Food not found.");
    }
}

void CatalogService::removeFood(int id) const {
    if (!foodStorage.remove(id)) {
        throw std::runtime_error("Food not found.");
    }
}
