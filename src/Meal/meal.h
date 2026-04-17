#pragma once

#include <string>
#include <vector>

#include "../Food/food.h"
#include "../Report/report.h"

struct MealItem {
    Food food;
    int grams;
};

class Meal {
private:
    int id;
    std::string name;
    std::vector<MealItem> items;

public:
    Meal();
    Meal(int id, const std::string& name);

    int getId() const;
    std::string getName() const;
    const std::vector<MealItem>& getItems() const;

    void addFood(const Food& food, int grams);
    Report buildReport() const;
};
