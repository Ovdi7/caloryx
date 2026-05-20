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
        // Категорія 1: Фрукти
        addFood(1, "Яблуко", 52, 0.3, 0.2, 14.0, 1);
        addFood(2, "Банан", 96, 1.3, 0.3, 27.0, 1);
        addFood(12, "Авокадо", 160, 2.0, 15.0, 9.0, 1);
        addFood(16, "Апельсин", 47, 0.9, 0.1, 12.0, 1);
        addFood(17, "Полуниця", 32, 0.7, 0.3, 7.7, 1);
        // Категорія 2: Білкові продукти
        addFood(3, "Куряча грудка", 165, 31.0, 3.6, 0.0, 2);
        addFood(7, "Яйце", 155, 13.0, 11.0, 1.1, 2);
        addFood(13, "Лосось", 206, 22.0, 12.0, 0.0, 2);
        addFood(15, "Тофу", 145, 16.0, 9.0, 2.0, 2);
        addFood(19, "Яловичина (фарш)", 217, 26.0, 11.0, 0.0, 2);
        // Категорія 3: Зернові
        addFood(4, "Рис (варений)", 130, 2.7, 0.3, 28.0, 3);
        addFood(8, "Вівсянка (суха)", 389, 16.9, 6.9, 66.3, 3);
        addFood(9, "Цільнозерновий хліб", 265, 13.0, 3.4, 48.0, 3);
        addFood(18, "Макарони (варені)", 131, 5.0, 1.0, 25.0, 3);
        // Категорія 4: Молочні продукти
        addFood(5, "Грецький йогурт", 59, 10.0, 0.4, 3.6, 4);
        addFood(10, "Молоко", 61, 3.2, 3.3, 4.8, 4);
        addFood(14, "Сир Чеддер", 404, 23.0, 33.0, 3.1, 4);
        // Категорія 5: Горіхи та жири
        addFood(6, "Мигдаль", 579, 21.2, 49.9, 21.6, 5);
        addFood(11, "Оливкова олія", 884, 0.0, 100.0, 0.0, 5);
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
