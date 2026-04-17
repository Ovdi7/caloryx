#include "CategoryStorage.h"

#include <fstream>
#include <stdexcept>

CategoryStorage::CategoryStorage(const std::string& filePath) : filePath(filePath) {
}

bool CategoryStorage::fileExists() const {
    std::ifstream input(filePath, std::ios::binary);
    return input.good();
}

void CategoryStorage::initializeEmpty() const {
    std::ofstream output(filePath, std::ios::binary | std::ios::trunc);
    if (!output) {
        throw std::runtime_error("Cannot initialize category storage.");
    }
}

void CategoryStorage::add(const CategoryData& category) const {
    std::ofstream output(filePath, std::ios::binary | std::ios::app);
    if (!output) {
        throw std::runtime_error("Cannot open category storage for append.");
    }

    output.write(reinterpret_cast<const char*>(&category), sizeof(CategoryData));
}

std::vector<CategoryData> CategoryStorage::getAll() const {
    std::vector<CategoryData> result;
    for (const CategoryData& category : loadRaw()) {
        if (!category.isDeleted) {
            result.push_back(category);
        }
    }
    return result;
}

bool CategoryStorage::findById(int id, CategoryData& result) const {
    for (const CategoryData& category : loadRaw()) {
        if (!category.isDeleted && category.id == id) {
            result = category;
            return true;
        }
    }

    return false;
}

bool CategoryStorage::update(const CategoryData& category) const {
    std::vector<CategoryData> categories = loadRaw();
    for (CategoryData& current : categories) {
        if (!current.isDeleted && current.id == category.id) {
            current = category;
            saveRaw(categories);
            return true;
        }
    }

    return false;
}

bool CategoryStorage::remove(int id) const {
    std::vector<CategoryData> categories = loadRaw();
    for (CategoryData& current : categories) {
        if (!current.isDeleted && current.id == id) {
            current.isDeleted = 1;
            saveRaw(categories);
            return true;
        }
    }

    return false;
}

std::vector<CategoryData> CategoryStorage::loadRaw() const {
    std::ifstream input(filePath, std::ios::binary);
    if (!input) {
        throw std::runtime_error("Cannot open category storage.");
    }

    std::vector<CategoryData> result;
    CategoryData record{};
    while (input.read(reinterpret_cast<char*>(&record), sizeof(CategoryData))) {
        result.push_back(record);
    }

    if (!input.eof()) {
        throw std::runtime_error("Category storage is corrupted.");
    }

    return result;
}

void CategoryStorage::saveRaw(const std::vector<CategoryData>& categories) const {
    std::ofstream output(filePath, std::ios::binary | std::ios::trunc);
    if (!output) {
        throw std::runtime_error("Cannot write category storage.");
    }

    for (const CategoryData& category : categories) {
        output.write(reinterpret_cast<const char*>(&category), sizeof(CategoryData));
    }
}
