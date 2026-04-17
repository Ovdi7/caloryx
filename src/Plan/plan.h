#pragma once

#include <string>
#include <vector>

#include "../Meal/meal.h"
#include "../Report/report.h"

class NutritionPlan {
private:
    int id;
    std::string name;
    std::vector<Meal> meals;

public:
    NutritionPlan();
    NutritionPlan(int id, const std::string& name);

    int getId() const;
    std::string getName() const;
    const std::vector<Meal>& getMeals() const;

    void addMeal(const Meal& meal);
    Report buildReport() const;
};
