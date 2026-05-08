#pragma once
#include "../Food/food.h"
#include <vector>
#include <string>

// Структура для зберігання одного запису про спожиту їжу
struct ConsumedFoodEntry {
    std::string foodName;
    double weightGrams;
    double calories;
    double protein;
    double fats;
    double carbs;
};

class DailyTracker {
private:
    double totalCaloriesConsumed = 0.0;
    double totalProteinConsumed = 0.0;
    double totalFatsConsumed = 0.0;
    double totalCarbsConsumed = 0.0;

    int dailyCalorieGoal = 0;
    double dailyProteinGoal = 0.0;
    double dailyFatsGoal = 0.0;
    double dailyCarbsGoal = 0.0;

    std::vector<ConsumedFoodEntry> consumedEntries;

public:
    void setDailyGoal(int goal, double weight = 0.0) {
        dailyCalorieGoal = goal;
        
        if (weight > 0) {
            // Професійний дієтологічний розрахунок за вагою тіла
            dailyProteinGoal = weight * 1.8; // 1.8 г білка на 1 кг ваги
            dailyFatsGoal = weight * 1.0;    // 1.0 г жиру на 1 кг ваги
            double remainingCalories = goal - (dailyProteinGoal * 4.0) - (dailyFatsGoal * 9.0);
            dailyCarbsGoal = (remainingCalories > 0 ? remainingCalories : 0) / 4.0;
        } else {
            // Запасний варіант (класичний розподіл 20/30/50), якщо вага невідома
            dailyProteinGoal = (goal * 0.20) / 4.0;
            dailyFatsGoal = (goal * 0.30) / 9.0;
            dailyCarbsGoal = (goal * 0.50) / 4.0;
        }
    }

    void consumeFood(const Food& food, double weightGrams) {
        // Розраховуємо нутрієнти для спожитої порції за пропорцією
        const double ratio = weightGrams / 100.0;
        double portionCalories = food.getCaloriesPer100g() * ratio;
        double portionProtein = food.getProteinsPer100g() * ratio;
        double portionFats = food.getFatsPer100g() * ratio;
        double portionCarbs = food.getCarbsPer100g() * ratio;

        // Оновлюємо загальну суму за день
        totalCaloriesConsumed += portionCalories;
        totalProteinConsumed += portionProtein;
        totalFatsConsumed += portionFats;
        totalCarbsConsumed += portionCarbs;

        // Додаємо запис у щоденник
        consumedEntries.push_back({
            food.getName(),
            weightGrams,
            portionCalories, portionProtein, portionFats, portionCarbs
        });
    }

    int getDailyGoal() const {
        return dailyCalorieGoal;
    }

    double getDailyProteinGoal() const {
        return dailyProteinGoal;
    }

    double getDailyFatsGoal() const {
        return dailyFatsGoal;
    }

    double getDailyCarbsGoal() const {
        return dailyCarbsGoal;
    }

    double getTotalConsumed() const {
        return totalCaloriesConsumed;
    }

    double getTotalProteinConsumed() const {
        return totalProteinConsumed;
    }

    double getTotalFatsConsumed() const {
        return totalFatsConsumed;
    }

    double getTotalCarbsConsumed() const {
        return totalCarbsConsumed;
    }

    double getCalorieDifference() const {
        // Якщо результат додатний — користувачу потрібно "добрати" калорії
        // Якщо від'ємний — користувач перебрав норму і їх потрібно "скинути"
        return dailyCalorieGoal - totalCaloriesConsumed;
    }

    double getProteinDifference() const {
        return dailyProteinGoal - totalProteinConsumed;
    }

    double getFatsDifference() const {
        return dailyFatsGoal - totalFatsConsumed;
    }

    double getCarbsDifference() const {
        return dailyCarbsGoal - totalCarbsConsumed;
    }

    const std::vector<ConsumedFoodEntry>& getConsumedEntries() const {
        return consumedEntries;
    }
};