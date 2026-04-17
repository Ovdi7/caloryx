#include "meal.h"

Meal::Meal() : id(0), name("Unnamed meal") {
}

Meal::Meal(int id, const std::string& name) : id(id), name(name) {
}

int Meal::getId() const {
    return id;
}

std::string Meal::getName() const {
    return name;
}

const std::vector<MealItem>& Meal::getItems() const {
    return items;
}

void Meal::addFood(const Food& food, int grams) {
    items.push_back({food, grams});
}

Report Meal::buildReport() const {
    double calories = 0.0;
    double proteins = 0.0;
    double fats = 0.0;
    double carbs = 0.0;

    for (const MealItem& item : items) {
        calories += item.food.calculateCalories(item.grams);
        proteins += item.food.calculateProteins(item.grams);
        fats += item.food.calculateFats(item.grams);
        carbs += item.food.calculateCarbs(item.grams);
    }

    return Report(calories, proteins, fats, carbs);
}
