#pragma once

#include <string>
#include <vector>

#include "../Models/FoodData.h"

class FoodStorage {
private:
    std::string filePath;

    std::vector<FoodData> loadRaw() const;
    void saveRaw(const std::vector<FoodData>& foods) const;

public:
    explicit FoodStorage(const std::string& filePath);

    bool fileExists() const;
    void initializeEmpty() const;

    void add(const FoodData& food) const;
    std::vector<FoodData> getAll() const;
    bool findById(int id, FoodData& result) const;
    bool update(const FoodData& food) const;
    bool remove(int id) const;
};
