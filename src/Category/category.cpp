#include "category.h"

#include "../Common/FixedText.h"

Category::Category() : data{} {
}

Category::Category(const CategoryData& data) : data(data) {
}

Category::Category(int id, const std::string& name) : data{} {
    data.id = id;
    copyText(data.name, CategoryData::NameSize, name);
    data.isDeleted = 0;
}

CategoryData Category::toData() const {
    return data;
}

int Category::getId() const {
    return data.id;
}

std::string Category::getName() const {
    return toString(data.name);
}
