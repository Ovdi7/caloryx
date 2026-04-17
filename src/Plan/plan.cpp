#include "plan.h"

NutritionPlan::NutritionPlan() : id(0), name("Default plan") {
}

NutritionPlan::NutritionPlan(int id, const std::string& name) : id(id), name(name) {
}

int NutritionPlan::getId() const {
    return id;
}

std::string NutritionPlan::getName() const {
    return name;
}

const std::vector<Meal>& NutritionPlan::getMeals() const {
    return meals;
}

void NutritionPlan::addMeal(const Meal& meal) {
    meals.push_back(meal);
}

Report NutritionPlan::buildReport() const {
    double calories = 0.0;
    double proteins = 0.0;
    double fats = 0.0;
    double carbs = 0.0;

    for (const Meal& meal : meals) {
        Report report = meal.buildReport();
        calories += report.getTotalCalories();
        proteins += report.getProteins();
        fats += report.getFats();
        carbs += report.getCarbs();
    }

    return Report(calories, proteins, fats, carbs);
}
