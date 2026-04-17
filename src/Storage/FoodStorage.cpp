#include "FoodStorage.h"

#include <fstream>
#include <stdexcept>

FoodStorage::FoodStorage(const std::string& filePath) : filePath(filePath) {
}

bool FoodStorage::fileExists() const {
    std::ifstream input(filePath, std::ios::binary);
    return input.good();
}

void FoodStorage::initializeEmpty() const {
    std::ofstream output(filePath, std::ios::binary | std::ios::trunc);
    if (!output) {
        throw std::runtime_error("Cannot initialize food storage.");
    }
}

void FoodStorage::add(const FoodData& food) const {
    std::ofstream output(filePath, std::ios::binary | std::ios::app);
    if (!output) {
        throw std::runtime_error("Cannot open food storage for append.");
    }

    output.write(reinterpret_cast<const char*>(&food), sizeof(FoodData));
}

std::vector<FoodData> FoodStorage::getAll() const {
    std::vector<FoodData> result;
    for (const FoodData& food : loadRaw()) {
        if (!food.isDeleted) {
            result.push_back(food);
        }
    }
    return result;
}

bool FoodStorage::findById(int id, FoodData& result) const {
    for (const FoodData& food : loadRaw()) {
        if (!food.isDeleted && food.id == id) {
            result = food;
            return true;
        }
    }

    return false;
}

bool FoodStorage::update(const FoodData& food) const {
    std::vector<FoodData> foods = loadRaw();
    for (FoodData& current : foods) {
        if (!current.isDeleted && current.id == food.id) {
            current = food;
            saveRaw(foods);
            return true;
        }
    }

    return false;
}

bool FoodStorage::remove(int id) const {
    std::vector<FoodData> foods = loadRaw();
    for (FoodData& current : foods) {
        if (!current.isDeleted && current.id == id) {
            current.isDeleted = 1;
            saveRaw(foods);
            return true;
        }
    }

    return false;
}

std::vector<FoodData> FoodStorage::loadRaw() const {
    std::ifstream input(filePath, std::ios::binary);
    if (!input) {
        throw std::runtime_error("Cannot open food storage.");
    }

    std::vector<FoodData> result;
    FoodData record{};
    while (input.read(reinterpret_cast<char*>(&record), sizeof(FoodData))) {
        result.push_back(record);
    }

    if (!input.eof()) {
        throw std::runtime_error("Food storage is corrupted.");
    }

    return result;
}

void FoodStorage::saveRaw(const std::vector<FoodData>& foods) const {
    std::ofstream output(filePath, std::ios::binary | std::ios::trunc);
    if (!output) {
        throw std::runtime_error("Cannot write food storage.");
    }

    for (const FoodData& food : foods) {
        output.write(reinterpret_cast<const char*>(&food), sizeof(FoodData));
    }
}
