#pragma once

#include <string>

#include "../Models/FoodData.h"

class Food {
private:
    FoodData data;

public:
    Food();
    explicit Food(const FoodData& data);
    Food(
        int id,
        const std::string& name,
        int caloriesPer100g,
        double proteinsPer100g,
        double fatsPer100g,
        double carbsPer100g,
        int categoryId
    );

    FoodData toData() const;

    int getId() const;
    std::string getName() const;
    int getCaloriesPer100g() const;
    double getProteinsPer100g() const;
    double getFatsPer100g() const;
    double getCarbsPer100g() const;
    int getCategoryId() const;

    double calculateCalories(int grams) const;
    double calculateProteins(int grams) const;
    double calculateFats(int grams) const;
    double calculateCarbs(int grams) const;
};
