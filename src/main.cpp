#include <exception>
#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <ctime>

#ifdef _WIN32
#include <windows.h>
#endif

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
#include "Models/DailyTracker.h"
#include "User/user.h"
#include "Services/AuthService.h"
#include "Services/CatalogService.h"
#include "Storage/AccountStorage.h"
#include "Storage/FoodStorage.h"
#include "Storage/BinaryStorage.h"
#include "Models/DailyLogData.h"

namespace {
const char* AccountsPath = "data/accounts.dat";
const char* FoodsPath = "data/foods.dat";
const char* LogsPath = "data/logs.dat";

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
    
    // Видаляємо невидимий символ \r (Carriage Return), який додає Windows, та зайві пробіли з кінця
    while (!value.empty() && (value.back() == '\r' || value.back() == ' ' || value.back() == '\t')) {
        value.pop_back();
    }
    
    // Видаляємо пробіли з початку (якщо випадково ввели перед словом)
    size_t start = value.find_first_not_of(" \t");
    if (start != std::string::npos) {
        value = value.substr(start);
    } else {
        value.clear();
    }
    
    return value;
}

// Функція для коректного переведення українських літер (UTF-8) у нижній регістр
std::string utf8ToLower(const std::string& str) {
    std::string result;
    result.reserve(str.length() * 2); // Резервуємо місце, бо 1-байтове кодування може стати 2-байтовим
    for (size_t i = 0; i < str.length(); ) {
        unsigned char c = str[i];
        
        if (c < 128) {
            result += std::tolower(c);
            ++i;
        } 
        else if ((c & 0xE0) == 0xC0 && i + 1 < str.length() && (str[i+1] & 0xC0) == 0x80) { 
            // Це валідний UTF-8 (2 байти)
            unsigned char b1 = str[i];
            unsigned char b2 = str[i+1];
            
            if (b1 == 0xD0 && b2 >= 0x90 && b2 <= 0x9F) { // А-П
                result += (char)0xD0; result += (char)(b2 + 0x20);
            } else if (b1 == 0xD0 && b2 >= 0xA0 && b2 <= 0xAF) { // Р-Я
                result += (char)0xD1; result += (char)(b2 - 0x20);
            } else if (b1 == 0xD0 && b2 == 0x86) { // І -> і
                result += (char)0xD1; result += (char)0x96;
            } else if (b1 == 0xD0 && b2 == 0x87) { // Ї -> ї
                result += (char)0xD1; result += (char)0x97;
            } else if (b1 == 0xD0 && b2 == 0x84) { // Є -> є
                result += (char)0xD1; result += (char)0x94;
            } else if (b1 == 0xD2 && b2 == 0x90) { // Ґ -> ґ
                result += (char)0xD2; result += (char)0x91;
            } else {
                result += (char)b1; result += (char)b2; // Вже малі літери
            }
            i += 2;
        } 
        else if ((c & 0xF0) == 0xE0 && i + 2 < str.length()) { result += str.substr(i, 3); i += 3; }
        else if ((c & 0xF8) == 0xF0 && i + 3 < str.length()) { result += str.substr(i, 4); i += 4; }
        else {
            // Консоль підсунула Windows-1251. Конвертуємо у малі літери UTF-8 "на льоту"!
            if (c >= 0xC0 && c <= 0xDF) { // Великі А-Я
                unsigned char lower_c = c + 0x20;
                if (lower_c <= 0xEF) { result += (char)0xD0; result += (char)(lower_c - 0x30); }
                else { result += (char)0xD1; result += (char)(lower_c - 0x70); }
            }
            else if (c >= 0xE0 && c <= 0xEF) { result += (char)0xD0; result += (char)(c - 0x30); } // Малі а-п
            else if (c >= 0xF0 && c <= 0xFF) { result += (char)0xD1; result += (char)(c - 0x70); } // Малі р-я
            else if (c == 0xB2 || c == 0xB3) { result += (char)0xD1; result += (char)0x96; } // І, і
            else if (c == 0xAF || c == 0xBF) { result += (char)0xD1; result += (char)0x97; } // Ї, ї
            else if (c == 0xAA || c == 0xBA) { result += (char)0xD1; result += (char)0x94; } // Є, є
            else if (c == 0xA5 || c == 0xB4) { result += (char)0xD2; result += (char)0x91; } // Ґ, ґ
            else { result += (char)c; }
            ++i;
        }
    }
    return result;
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
              << "Age: " << AuthService::calculateAge(account.birthYear, account.birthMonth, account.birthDay) << " (DOB: " << account.birthDay << "." << account.birthMonth << "." << account.birthYear << ")\n"
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

void runUserMenu(const AccountData& account, CatalogService& catalogService, AuthService& authService, BinaryStorage<DailyLogData>& logStorage) {
    User user(account);
    DailyTracker tracker;
    
    // Беремо ціль калорій, яка вже розрахована та збережена в акаунті користувача
    tracker.setDailyGoal(account.dailyCalories, account.weight);

    // Отримуємо поточну дату
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    int currentYear = now->tm_year + 1900;
    int currentMonth = now->tm_mon + 1;
    int currentDay = now->tm_mday;

    FoodDatabase database(catalogService);
    const std::vector<Food> allFoods = database.getFoods();

    // Завантажуємо історію ТІЛЬКИ за сьогоднішній день
    for (const DailyLogData& log : logStorage.getAll()) {
        if (log.accountId == account.id && log.year == currentYear && log.month == currentMonth && log.day == currentDay) {
            for (const Food& f : allFoods) {
                if (f.getId() == log.foodId) {
                    tracker.consumeFood(f, log.weightGrams);
                    break;
                }
            }
        }
    }

    bool running = true;
    while (running) {
        std::cout << "\n=== User Menu ===\n"
                  << "Logged in as: " << user.getDisplayName() << " (" << user.getLogin() << ")\n"
                  << "1. View my account\n"
                  << "2. Show foods\n"
                  << "3. Show categories\n"
                  << "4. Consume food\n"
                  << "5. View daily stats\n"
                  << "6. Update physical data\n"
                  << "7. View today's food log\n"
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
            case 4:
            {
                FoodDatabase database(catalogService);
                showFoods(database);
                
                const std::string foodName = readLine("Enter food name you consumed: ");
                std::string searchLower = utf8ToLower(foodName);
                
                const std::vector<Food> foods = database.getFoods();
                std::vector<Food> matches;
                
                for (const Food& f : foods) {
                    std::string nameLower = utf8ToLower(f.getName());
                    if (nameLower.find(searchLower) != std::string::npos) {
                        matches.push_back(f);
                    }
                }
                if (matches.empty()) {
                    std::cout << "Food matching '" << foodName << "' not found.\n";
                } else {
                    Food selectedFood = matches[0];
                    if (matches.size() > 1) {
                        std::cout << "\nMultiple foods found:\n";
                        for (size_t i = 0; i < matches.size(); ++i) {
                            std::cout << i + 1 << ". " << matches[i].getName() 
                                      << " (" << matches[i].getCaloriesPer100g() << " kcal/100g)\n";
                        }
                        int idx = readInt("Select food number: ");
                        if (idx > 0 && idx <= static_cast<int>(matches.size())) {
                            selectedFood = matches[idx - 1];
                        } else {
                            std::cout << "Invalid selection. Canceled.\n";
                            break;
                        }
                    }
                    
                    const double weight = readDouble("Enter weight in grams: ");
                    tracker.consumeFood(selectedFood, weight);
                    
                    // Зберігаємо запис у файл журналу
                    DailyLogData newLog{};
                    int maxId = 0;
                    for (const DailyLogData& log : logStorage.getAll()) {
                        if (log.id > maxId) maxId = log.id;
                    }
                    newLog.id = maxId + 1;
                    newLog.accountId = account.id;
                    newLog.year = currentYear;
                    newLog.month = currentMonth;
                    newLog.day = currentDay;
                    newLog.foodId = selectedFood.getId();
                    newLog.weightGrams = weight;
                    newLog.isDeleted = 0;
                    logStorage.add(newLog);
                    
                    std::cout << "Added " << weight << "g of " << selectedFood.getName() << " to your tracker.\n";
                }
                break;
            }
            case 5:
            {
                std::cout << "\n--- DAILY STATS ---\n"
                          << "[Calories] Goal: " << tracker.getDailyGoal() << " kcal | Consumed: " << tracker.getTotalConsumed() << " kcal\n"
                          << "-------------------\n"
                          << "[Protein]  Goal: " << tracker.getDailyProteinGoal() << " g | Consumed: " << tracker.getTotalProteinConsumed() << " g\n"
                          << "[Fats]     Goal: " << tracker.getDailyFatsGoal() << " g | Consumed: " << tracker.getTotalFatsConsumed() << " g\n"
                          << "[Carbs]    Goal: " << tracker.getDailyCarbsGoal() << " g | Consumed: " << tracker.getTotalCarbsConsumed() << " g\n"
                          << "-------------------\n";
                          
                double diff = tracker.getCalorieDifference();
                if (diff > 0) {
                    std::cout << "You need to consume: " << diff << " more kcal\n";
                } else if (diff < 0) {
                    std::cout << "You exceeded your goal by: " << -diff << " kcal\n";
                } else {
                    std::cout << "You perfectly reached your goal!\n";
                }
                
                auto printMacroStatus = [](const std::string& name, double difference) {
                    if (difference > 0) std::cout << "-> " << name << ": need " << difference << " g more\n";
                    else if (difference < 0) std::cout << "-> " << name << ": exceeded by " << -difference << " g\n";
                };
                printMacroStatus("Protein", tracker.getProteinDifference());
                printMacroStatus("Fats", tracker.getFatsDifference());
                printMacroStatus("Carbs", tracker.getCarbsDifference());
                
                std::cout << "-------------------\n";
                break;
            }
            case 6:
            {
                std::cout << "\n--- Update Physical Data ---\n";
                const double weight = readDouble("New weight (kg): ");
                const double height = readDouble("New height (cm): ");
                
                const int age = AuthService::calculateAge(account.birthYear, account.birthMonth, account.birthDay);

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

                const int newDailyCalories = CalorieCalculator::getRecommendedCalories(gender, weight, height, age, activity, goal);
                
                tracker.setDailyGoal(newDailyCalories, weight);
                
                // Передаємо логіку збереження на рівень сервісу
                authService.updatePhysicalData(account.id, weight, height, newDailyCalories);
                std::cout << "\n✅ Profile updated! New daily goal is " << newDailyCalories << " kcal (Calculated for age " << age << ").\n";
                break;
            }
            case 7:
            {
                std::cout << "\n--- TODAY'S FOOD LOG ---\n";
                const auto& entries = tracker.getConsumedEntries();
                if (entries.empty()) {
                    std::cout << "You haven't logged any food today.\n";
                } else {
                    for (const auto& entry : entries) {
                        std::cout << "- " << entry.foodName << " (" << entry.weightGrams << "g)\n"
                                  << "  Calories: " << entry.calories << " kcal, "
                                  << "P: " << entry.protein << "g, "
                                  << "F: " << entry.fats << "g, "
                                  << "C: " << entry.carbs << "g\n";
                    }
                }
                std::cout << "------------------------\n";
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
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    try {
        AccountStorage accountStorage(AccountsPath);
        FoodStorage foodStorage(FoodsPath);
        BinaryStorage<DailyLogData> logStorage(LogsPath);

        if (!logStorage.fileExists()) {
            logStorage.initializeEmpty();
        }

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
                            runUserMenu(account, catalogService, authService, logStorage);
                        }
                        break;
                    }
                    case 2: {
                        const std::string login = readLine("Login: ");
                        const std::string password = readLine("Password: ");
                        const std::string displayName = readLine("Display name: ");
                        const int birthYear = readInt("Birth Year (e.g. 1995): ");
                        const int birthMonth = readInt("Birth Month (1-12): ");
                        const int birthDay = readInt("Birth Day (1-31): ");
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

                        const int age = AuthService::calculateAge(birthYear, birthMonth, birthDay);
                        const int dailyCalories = CalorieCalculator::getRecommendedCalories(gender, weight, height, age, activity, goal);
                        std::cout << "\n============================================\n"
                                  << "--> Calculated daily calories: " << dailyCalories << " kcal <--\n"
                                  << "============================================\n\n";

                        const AccountData account = authService.registerUser(
                            login,
                            password,
                            displayName,
                            birthYear,
                            birthMonth,
                            birthDay,
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
