#include "food.h"

#include "../Common/FixedText.h"

Food::Food() : data{} {
}

Food::Food(const FoodData& data) : data(data) {
}

Food::Food(
    int id,
    const std::string& name,
    int caloriesPer100g,
    double proteinsPer100g,
    double fatsPer100g,
    double carbsPer100g,
    int categoryId
) : data{} {
    data.id = id;
    copyText(data.name, FoodData::NameSize, name);
    data.caloriesPer100g = caloriesPer100g;
    data.proteinsPer100g = proteinsPer100g;
    data.fatsPer100g = fatsPer100g;
    data.carbsPer100g = carbsPer100g;
    data.categoryId = categoryId;
    data.isDeleted = 0;
}

FoodData Food::toData() const {
    return data;
}

int Food::getId() const {
    return data.id;
}

std::string Food::getName() const {
    return toString(data.name);
}

int Food::getCaloriesPer100g() const {
    return data.caloriesPer100g;
}

double Food::getProteinsPer100g() const {
    return data.proteinsPer100g;
}

double Food::getFatsPer100g() const {
    return data.fatsPer100g;
}

double Food::getCarbsPer100g() const {
    return data.carbsPer100g;
}

int Food::getCategoryId() const {
    return data.categoryId;
}

double Food::calculateCalories(int grams) const {
    return static_cast<double>(data.caloriesPer100g) * grams / 100.0;
}

double Food::calculateProteins(int grams) const {
    return data.proteinsPer100g * grams / 100.0;
}

double Food::calculateFats(int grams) const {
    return data.fatsPer100g * grams / 100.0;
}

double Food::calculateCarbs(int grams) const {
    return data.carbsPer100g * grams / 100.0;
}
