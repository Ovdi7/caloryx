#pragma once

#include <string>
#include <vector>

#include "../Models/CategoryData.h"
#include "../Models/FoodData.h"
#include "../Storage/CategoryStorage.h"
#include "../Storage/FoodStorage.h"

class CatalogService {
private:
    FoodStorage& foodStorage;
    CategoryStorage& categoryStorage;

public:
    CatalogService(FoodStorage& foodStorage, CategoryStorage& categoryStorage);

    void ensureSeedData() const;

    std::vector<FoodData> getAllFoods() const;
    std::vector<CategoryData> getAllCategories() const;

    FoodData getFoodById(int id) const;
    CategoryData getCategoryById(int id) const;

    void addCategory(int id, const std::string& name) const;
    void removeCategory(int id) const;

    void addFood(
        int id,
        const std::string& name,
        int caloriesPer100g,
        double proteinsPer100g,
        double fatsPer100g,
        double carbsPer100g,
        int categoryId
    ) const;
    void updateFood(
        int id,
        const std::string& name,
        int caloriesPer100g,
        double proteinsPer100g,
        double fatsPer100g,
        double carbsPer100g,
        int categoryId
    ) const;
    void removeFood(int id) const;
};
