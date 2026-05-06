#pragma once

#include <string>
#include <vector>

#include "../Models/CategoryData.h"
#include "../Models/FoodData.h"
#include "../Storage/FoodStorage.h"

class CatalogService {
private:
    FoodStorage& foodStorage;

public:
    explicit CatalogService(FoodStorage& foodStorage);

    void ensureSeedData() const;

    std::vector<FoodData> getAllFoods() const;

    FoodData getFoodById(int id) const;

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
