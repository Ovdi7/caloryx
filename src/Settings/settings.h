#pragma once

class Settings {
private:
    int userId;
    int dailyCalories;

public:
    Settings();
    Settings(int userId, int dailyCalories);

    int getUserId() const;
    int getDailyCalories() const;
    void setDailyCalories(int value);
};
