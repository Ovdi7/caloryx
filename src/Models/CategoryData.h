#pragma once

enum class Category {
    Fruits = 1,
    Protein = 2,
    Grains = 3,
    Dairy = 4,
    Nuts = 5
};

struct CategoryData {
    static const int NameSize = 64;

    int id;
    char name[NameSize];
    int isDeleted;
};
