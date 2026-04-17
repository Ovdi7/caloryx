#pragma once

#include <vector>

#include "../Category/category.h"
#include "../Food/food.h"
#include "../Services/CatalogService.h"

class FoodDatabase {
private:
    CatalogService* catalogService;

public:
    FoodDatabase();
    explicit FoodDatabase(CatalogService& catalogService);

    void bind(CatalogService& catalogService);

    std::vector<Food> getFoods() const;
    std::vector<Category> getCategories() const;
    Food getFoodById(int id) const;
};
