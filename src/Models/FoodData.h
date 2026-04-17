#pragma once

struct FoodData {
    static const int NameSize = 64;

    int id;
    char name[NameSize];
    int caloriesPer100g;
    double proteinsPer100g;
    double fatsPer100g;
    double carbsPer100g;
    int categoryId;
    int isDeleted;
};
