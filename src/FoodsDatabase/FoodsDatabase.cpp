#include "FoodsDatabase.h"

FoodDatabase::FoodDatabase() : catalogService(nullptr) {
}

FoodDatabase::FoodDatabase(CatalogService& catalogService) : catalogService(&catalogService) {
}

void FoodDatabase::bind(CatalogService& service) {
    catalogService = &service;
}

std::vector<Food> FoodDatabase::getFoods() const {
    std::vector<Food> result;
    if (catalogService == nullptr) {
        return result;
    }

    for (const FoodData& food : catalogService->getAllFoods()) {
        result.push_back(Food(food));
    }

    return result;
}

std::vector<Category> FoodDatabase::getCategories() const {
    std::vector<Category> result;
    if (catalogService == nullptr) {
        return result;
    }

    for (const CategoryData& category : catalogService->getAllCategories()) {
        result.push_back(Category(category));
    }

    return result;
}

Food FoodDatabase::getFoodById(int id) const {
    return Food(catalogService->getFoodById(id));
}
