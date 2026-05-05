#pragma once

enum class Goal {
    LoseWeight,
    MaintainWeight,
    GainWeight
};

enum class ActivityLevel {
    Sedentary,     // Мінімальна активність, сидяча робота
    Light,         // Легкі тренування 1-3 рази на тиждень
    Moderate,      // Помірні тренування 3-5 разів на тиждень
    Active,        // Інтенсивні тренування 6-7 разів на тиждень
    VeryActive     // Важка фізична робота або тренування 2 рази на день
};

enum class Gender {
    Male,
    Female
};

class CalorieCalculator {
public:
    // Базовий обмін речовин (BMR)
    static double calculateBMR(Gender gender, double weightKg, double heightCm, int ageYears);
    // Загальні витрати енергії (TDEE) з урахуванням активності
    static double calculateTDEE(double bmr, ActivityLevel activityLevel);
    // Підсумкова кількість калорій з урахуванням цілі (схуднення, підтримка, набір)
    static int getRecommendedCalories(Gender gender, double weightKg, double heightCm, int ageYears, ActivityLevel activityLevel, Goal goal);
};