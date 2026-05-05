#include "category.h"

std::string getCategoryName(Category category) {
    switch (category) {
        case Category::Fruits: return "Fruits";
        case Category::Protein: return "Protein";
        case Category::Grains: return "Grains";
        case Category::Dairy: return "Dairy";
        case Category::Nuts: return "Nuts";
        default: return "Unknown";
    }
}

std::vector<Category> getAllCategories() {
    return {
        Category::Fruits,
        Category::Protein,
        Category::Grains,
        Category::Dairy,
        Category::Nuts
    };
}

bool isValidCategory(int id) {
    return id >= 1 && id <= 5;
}
