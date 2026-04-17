#include "settings.h"

Settings::Settings() : userId(0), dailyCalories(2000) {
}

Settings::Settings(int userId, int dailyCalories) : userId(userId), dailyCalories(dailyCalories) {
}

int Settings::getUserId() const {
    return userId;
}

int Settings::getDailyCalories() const {
    return dailyCalories;
}

void Settings::setDailyCalories(int value) {
    dailyCalories = value;
}
