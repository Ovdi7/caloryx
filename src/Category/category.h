#pragma once

#include <string>
#include <vector>

#include "../Models/CategoryData.h"

std::string getCategoryName(Category category);
std::vector<Category> getAllCategories();
bool isValidCategory(int id);
