#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>

template <typename T>
class BinaryStorage {
private:
    std::string filePath;

public:
    explicit BinaryStorage(const std::string& filePath) : filePath(filePath) {}

    bool fileExists() const {
        std::ifstream input(filePath, std::ios::binary);
        return input.good();
    }

    void initializeEmpty() const {
        std::ofstream output(filePath, std::ios::binary | std::ios::trunc);
        if (!output) {
            throw std::runtime_error("Cannot initialize storage.");
        }
    }

    void add(const T& item) const {
        std::ofstream output(filePath, std::ios::binary | std::ios::app);
        if (!output) {
            throw std::runtime_error("Cannot open storage for append.");
        }
        output.write(reinterpret_cast<const char*>(&item), sizeof(T));
    }

    std::vector<T> getAll() const {
        std::ifstream input(filePath, std::ios::binary);
        std::vector<T> result;
        if (!input) return result;

        T record{};
        while (input.read(reinterpret_cast<char*>(&record), sizeof(T))) {
            if (!record.isDeleted) {
                result.push_back(record);
            }
        }
        return result;
    }

    bool findById(int id, T& result) const {
        std::ifstream input(filePath, std::ios::binary);
        if (!input) return false;

        T record{};
        while (input.read(reinterpret_cast<char*>(&record), sizeof(T))) {
            if (!record.isDeleted && record.id == id) {
                result = record;
                return true;
            }
        }
        return false;
    }

    bool update(const T& item) const {
        std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
        if (!file) return false;

        T record{};
        while (file.read(reinterpret_cast<char*>(&record), sizeof(T))) {
            if (!record.isDeleted && record.id == item.id) {
                // Повертаємось на один запис назад і перезаписуємо тільки його
                file.seekp(-static_cast<std::streamoff>(sizeof(T)), std::ios::cur);
                file.write(reinterpret_cast<const char*>(&item), sizeof(T));
                return true;
            }
        }
        return false;
    }

    bool remove(int id) const {
        std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
        if (!file) return false;

        T record{};
        while (file.read(reinterpret_cast<char*>(&record), sizeof(T))) {
            if (!record.isDeleted && record.id == id) {
                record.isDeleted = 1; // "М'яке" видалення
                file.seekp(-static_cast<std::streamoff>(sizeof(T)), std::ios::cur);
                file.write(reinterpret_cast<const char*>(&record), sizeof(T));
                return true;
            }
        }
        return false;
    }
};