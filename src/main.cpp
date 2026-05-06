#include <exception>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "Admin/admin.h"
#include "Category/category.h"
#include "Common/FixedText.h"
#include "Food/food.h"
#include "FoodsDatabase/FoodsDatabase.h"
#include "Models/AccountData.h"
#include "Models/AccountRole.h"
#include "Models/CalorieCalculator.h"
#include "Models/CategoryData.h"
#include "Models/FoodData.h"
#include "User/user.h"
#include "Services/AuthService.h"
#include "Services/CatalogService.h"
#include "Storage/AccountStorage.h"
#include "Storage/FoodStorage.h"

namespace {
const char* AccountsPath = "data/accounts.dat";
const char* FoodsPath = "data/foods.dat";

int readInt(const std::string& label) {
    int value = 0;
    std::cout << label;
    while (!(std::cin >> value)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid number. Try again: ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return value;
}

double readDouble(const std::string& label) {
    double value = 0.0;
    std::cout << label;
    while (!(std::cin >> value)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid number. Try again: ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return value;
}

std::string readLine(const std::string& label) {
    std::string value;
    std::cout << label;
    std::getline(std::cin, value);
    return value;
}

void showFoods(const FoodDatabase& database) {
    const std::vector<Food> foods = database.getFoods();
    for (const Food& food : foods) {
        std::cout << "Food #" << food.getId() << " - " << food.getName() << '\n'
                  << "Calories: " << food.getCaloriesPer100g() << ", "
                  << "P: " << food.getProteinsPer100g() << ", "
                  << "F: " << food.getFatsPer100g() << ", "
                  << "C: " << food.getCarbsPer100g() << ", "
                  << "Category ID: " << food.getCategoryId() << "\n\n";
    }
}

void showCategories(const FoodDatabase& database) {
    const std::vector<Category> categories = database.getCategories();
    for (const Category& category : categories) {
        std::cout << "Category #" << static_cast<int>(category) << " - " << getCategoryName(category) << '\n';
    }
}

void showAccountProfile(const AccountData& account) {
    std::cout << "Account #" << account.id << '\n'
              << "Login: " << account.login << '\n'
              << "Display name: " << account.displayName << '\n'
              << "Role: " << (account.role == RoleAdmin ? "admin" : "user") << '\n'
              << "Age: " << account.age << '\n'
              << "Weight: " << account.weight << '\n'
              << "Height: " << account.height << '\n'
              << "Daily calories: " << account.dailyCalories << '\n';
}

void runAdminMenu(const AccountData& account, CatalogService& catalogService, FoodDatabase& database) {
    Admin admin(account);
    bool running = true;
    while (running) {
        std::cout << "\n=== Admin Menu ===\n"
                  << "Logged in as: " << admin.getDisplayName() << " (" << admin.getLogin() << ")\n"
                  << "1. Show foods\n"
                  << "2. Show categories\n"
                  << "3. Add food\n"
                  << "4. Update food\n"
                  << "5. Delete food\n"
                  << "0. Logout\n";

        const int choice = readInt("Choose action: ");
        switch (choice) {
            case 1:
                showFoods(database);
                break;
            case 2:
                showCategories(database);
                break;
            case 3:
            {
                const int id = readInt("Food ID: ");
                const std::string name = readLine("Food name: ");
                const int calories = readInt("Calories per 100g: ");
                const double proteins = readDouble("Proteins per 100g: ");
                const double fats = readDouble("Fats per 100g: ");
                const double carbs = readDouble("Carbs per 100g: ");
                const int categoryId = readInt("Category ID: ");
                catalogService.addFood(id, name, calories, proteins, fats, carbs, categoryId);
                std::cout << "Food added.\n";
                break;
            }
            case 4:
            {
                const int id = readInt("Food ID: ");
                const std::string name = readLine("New food name: ");
                const int calories = readInt("New calories per 100g: ");
                const double proteins = readDouble("New proteins per 100g: ");
                const double fats = readDouble("New fats per 100g: ");
                const double carbs = readDouble("New carbs per 100g: ");
                const int categoryId = readInt("New category ID: ");
                catalogService.updateFood(id, name, calories, proteins, fats, carbs, categoryId);
                std::cout << "Food updated.\n";
                break;
            }
            case 5:
                catalogService.removeFood(readInt("Food ID: "));
                std::cout << "Food deleted.\n";
                break;
            case 0:
                running = false;
                break;
            default:
                std::cout << "Unknown option.\n";
                break;
        }
    }
}

void runUserMenu(const AccountData& account, CatalogService& catalogService, AuthService& authService) {
    User user(account);
    bool running = true;
    while (running) {
        std::cout << "\n=== User Menu ===\n"
                  << "Logged in as: " << user.getDisplayName() << " (" << user.getLogin() << ")\n"
                  << "1. View my account\n"
                  << "2. Show foods\n"
                  << "3. Show categories\n"
                  << "0. Logout\n";

        const int choice = readInt("Choose action: ");
        switch (choice) {
            case 1:
                showAccountProfile(authService.getAccountById(account.id));
                break;
            case 2:
            {
                FoodDatabase database(catalogService);
                showFoods(database);
                break;
            }
            case 3:
            {
                FoodDatabase database(catalogService);
                showCategories(database);
                break;
            }
            case 0:
                running = false;
                break;
            default:
                std::cout << "Unknown option.\n";
                break;
        }
    }
}
}

int main() {
    try {
        AccountStorage accountStorage(AccountsPath);
        FoodStorage foodStorage(FoodsPath);

        AuthService authService(accountStorage);
        CatalogService catalogService(foodStorage);
        FoodDatabase database(catalogService);

        authService.ensureSeedData();
        catalogService.ensureSeedData();

        bool running = true;
        while (running) {
            std::cout << "\n=== Caloryx ===\n"
                      << "1. Login\n"
                      << "2. Register user\n"
                      << "3. Show accounts\n"
                      << "0. Exit\n";

            const int choice = readInt("Choose action: ");
            try {
                switch (choice) {
                    case 1: {
                        const std::string login = readLine("Login: ");
                        const std::string password = readLine("Password: ");
                        const AccountData account = authService.login(login, password);

                        if (account.role == RoleAdmin) {
                            runAdminMenu(account, catalogService, database);
                        } else {
                            runUserMenu(account, catalogService, authService);
                        }
                        break;
                    }
                    case 2: {
                        const std::string login = readLine("Login: ");
                        const std::string password = readLine("Password: ");
                        const std::string displayName = readLine("Display name: ");
                        const int age = readInt("Age: ");
                        const double weight = readDouble("Weight: ");
                        const double height = readDouble("Height: ");
                        
                        std::cout << "\nGender:\n"
                                  << "0. Male\n"
                                  << "1. Female\n";
                        const int genderInput = readInt("Choice: ");
                        Gender gender = (genderInput == 1) ? Gender::Female : Gender::Male;

                        std::cout << "\nActivity Level:\n"
                                  << "0. Sedentary (office job)\n"
                                  << "1. Light (1-3 workouts/week)\n"
                                  << "2. Moderate (3-5 workouts/week)\n"
                                  << "3. Active (6-7 workouts/week)\n"
                                  << "4. Very Active (physical job or 2x/day workouts)\n";
                        const int activityInput = readInt("Choice: ");
                        ActivityLevel activity = static_cast<ActivityLevel>(activityInput >= 0 && activityInput <= 4 ? activityInput : 0);

                        std::cout << "\nGoal:\n"
                                  << "0. Lose Weight\n"
                                  << "1. Maintain Weight\n"
                                  << "2. Gain Weight\n";
                        const int goalInput = readInt("Choice: ");
                        Goal goal = static_cast<Goal>(goalInput >= 0 && goalInput <= 2 ? goalInput : 1);

                        const int dailyCalories = CalorieCalculator::getRecommendedCalories(gender, weight, height, age, activity, goal);
                        std::cout << "\n============================================\n"
                                  << "--> Calculated daily calories: " << dailyCalories << " kcal <--\n"
                                  << "============================================\n\n";

                        const AccountData account = authService.registerUser(
                            login,
                            password,
                            displayName,
                            age,
                            weight,
                            height,
                            dailyCalories
                        );
                        std::cout << "Registration completed.\n";
                        showAccountProfile(account);
                        break;
                    }
                    case 3: {
                        const std::vector<AccountData> accounts = authService.getAllAccounts();
                        for (const AccountData& account : accounts) {
                            std::cout << account.login << " (" << (account.role == RoleAdmin ? "admin" : "user") << ")\n";
                        }
                        break;
                    }
                    case 0:
                        running = false;
                        break;
                    default:
                        std::cout << "Unknown option.\n";
                        break;
                }
            } catch (const std::exception& error) {
                std::cout << "Operation failed: " << error.what() << '\n';
            }
        }

        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Fatal error: " << error.what() << '\n';
        return 1;
    }
}
