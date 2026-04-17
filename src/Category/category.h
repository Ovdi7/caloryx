#pragma once

#include <string>

#include "../Models/CategoryData.h"

class Category {
private:
    CategoryData data;

public:
    Category();
    explicit Category(const CategoryData& data);
    Category(int id, const std::string& name);

    CategoryData toData() const;

    int getId() const;
    std::string getName() const;
};
