#pragma once

#include <string>

#include "../Meal/meal.h"
#include "../Models/AccountData.h"
#include "../Plan/plan.h"
#include "../Settings/settings.h"

class User {
private:
    AccountData account;
    Settings settings;
    NutritionPlan nutritionPlan;

public:
    User();
    explicit User(const AccountData& account);

    int getId() const;
    std::string getLogin() const;
    std::string getDisplayName() const;
    Settings getSettings() const;
    const NutritionPlan& getNutritionPlan() const;

    void setNutritionPlan(const NutritionPlan& value);
};
