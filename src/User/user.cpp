#include "user.h"

#include "../Common/FixedText.h"

User::User() : account{}, settings(), nutritionPlan() {
}

User::User(const AccountData& account)
    : account(account),
      settings(account.id, account.dailyCalories),
      nutritionPlan(1, toString(account.displayName) + "'s plan") {
}

int User::getId() const {
    return account.id;
}

std::string User::getLogin() const {
    return toString(account.login);
}

std::string User::getDisplayName() const {
    return toString(account.displayName);
}

Settings User::getSettings() const {
    return settings;
}

const NutritionPlan& User::getNutritionPlan() const {
    return nutritionPlan;
}

void User::setNutritionPlan(const NutritionPlan& value) {
    nutritionPlan = value;
}
