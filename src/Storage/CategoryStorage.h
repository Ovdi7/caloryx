#pragma once

#include <string>
#include <vector>

#include "../Models/CategoryData.h"

class CategoryStorage {
private:
    std::string filePath;

    std::vector<CategoryData> loadRaw() const;
    void saveRaw(const std::vector<CategoryData>& categories) const;

public:
    explicit CategoryStorage(const std::string& filePath);

    bool fileExists() const;
    void initializeEmpty() const;

    void add(const CategoryData& category) const;
    std::vector<CategoryData> getAll() const;
    bool findById(int id, CategoryData& result) const;
    bool update(const CategoryData& category) const;
    bool remove(int id) const;
};
