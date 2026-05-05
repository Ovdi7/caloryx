#include "CalorieCalculator.h"
#include <cmath>

double CalorieCalculator::calculateBMR(Gender gender, double weightKg, double heightCm, int ageYears) {
    // Формула Міффліна-Сан Жеора
    double bmr = (10.0 * weightKg) + (6.25 * heightCm) - (5.0 * ageYears);
    
    if (gender == Gender::Male) {
        bmr += 5.0;
    } else {
        bmr -= 161.0;
    }
    
    return bmr;
}

double CalorieCalculator::calculateTDEE(double bmr, ActivityLevel activityLevel) {
    switch (activityLevel) {
        case ActivityLevel::Sedentary:  return bmr * 1.2;
        case ActivityLevel::Light:      return bmr * 1.375;
        case ActivityLevel::Moderate:   return bmr * 1.55;
        case ActivityLevel::Active:     return bmr * 1.725;
        case ActivityLevel::VeryActive: return bmr * 1.9;
        default:                        return bmr * 1.2;
    }
}

int CalorieCalculator::getRecommendedCalories(Gender gender, double weightKg, double heightCm, int ageYears, ActivityLevel activityLevel, Goal goal) {
    double bmr = calculateBMR(gender, weightKg, heightCm, ageYears);
    double tdee = calculateTDEE(bmr, activityLevel);
    
    double finalCalories = tdee;
    
    switch (goal) {
        case Goal::LoseWeight:
            finalCalories -= 500.0; // Дефіцит для безпечного схуднення (~0.5 кг на тиждень)
            break;
        case Goal::GainWeight:
            finalCalories += 500.0; // Профіцит для набору маси
            break;
        case Goal::MaintainWeight:
        default:
            break; // Підтримка ваги не потребує змін TDEE
    }
    
    // Не рекомендується опускати калораж нижче 1200 для жінок і 1500 для чоловіків, але для простоти поки повертаємо як є
    return static_cast<int>(std::round(finalCalories));
}