#include "FoodsDatabase.h"
#include "../Models/FoodData.h"
#include "../Models/CategoryData.h"

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

    result.push_back(Category::Fruits);
    result.push_back(Category::Protein);
    result.push_back(Category::Grains);
    result.push_back(Category::Dairy);
    result.push_back(Category::Nuts);

    return result;
}

Food FoodDatabase::getFoodById(int id) const {
    return Food(catalogService->getFoodById(id));
}
