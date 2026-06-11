#include "crow_all.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <map>
#include "Models/CalorieCalculator.h"

const std::string USERS_FILE    = "users.dat";
const std::string PRODUCTS_FILE = "products.dat";

struct User {
    int id;
    char name[64];
    char email[64];
    char password[64];
    char gender[16];
    int age;
    double height;
    double weight;
    char activity_level[32];
    char goal[32];
};

struct Product {
    int id;
    int user_id;
    char name[64];
    double weight;
    double calories;
    char category[64];
    char time[16];
    char date[16];
};

template<typename T>
std::vector<T> readBinary(const std::string& filename) {
    std::vector<T> data;
    std::ifstream in(filename, std::ios::binary | std::ios::ate);
    if (in) {
        std::streamsize size = in.tellg();
        in.seekg(0, std::ios::beg);
        if (size > 0) {
            data.resize(size / sizeof(T));
            in.read(reinterpret_cast<char*>(data.data()), size);
        }
    }
    return data;
}

template<typename T>
void appendBinary(const std::string& filename, const T& item) {
    std::ofstream out(filename, std::ios::binary | std::ios::app);
    if (out) {
        out.write(reinterpret_cast<const char*>(&item), sizeof(T));
    }
}

void copyStr(char* dest, const std::string& src, size_t max_len) {
    std::strncpy(dest, src.c_str(), max_len - 1);
    dest[max_len - 1] = '\0';
}

int main() {
    crow::App<crow::CORSHandler> app;
    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors.global()
        .headers("*")
        .methods("POST"_method, "GET"_method, "OPTIONS"_method)
        .origin("*");

    // ── /api/register ───────────────────────────────────────────────────────
    CROW_ROUTE(app, "/api/register").methods("POST"_method)([](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");

        auto users = readBinary<User>(USERS_FILE);
        User u{};
        u.id = users.empty() ? 1 : users.back().id + 1;
        copyStr(u.name,           body["name"].s(),           sizeof(u.name));
        copyStr(u.email,          body["email"].s(),          sizeof(u.email));
        copyStr(u.password,       body["password"].s(),       sizeof(u.password));
        copyStr(u.gender,         body["gender"].s(),         sizeof(u.gender));
        u.age    = body["age"].i();
        u.height = body["height"].d();
        u.weight = body["weight"].d();
        copyStr(u.activity_level, body["activity_level"].s(), sizeof(u.activity_level));
        copyStr(u.goal,           body["goal"].s(),           sizeof(u.goal));

        appendBinary(USERS_FILE, u);

        crow::json::wvalue res;
        res["id"]             = u.id;
        res["name"]           = std::string(u.name);
        res["email"]          = std::string(u.email);
        res["age"]            = u.age;
        res["height"]         = u.height;
        res["weight"]         = u.weight;
        res["gender"]         = std::string(u.gender);
        res["activity_level"] = std::string(u.activity_level);
        res["goal"]           = std::string(u.goal);
        return crow::response(200, res);
    });

    // ── /api/login ──────────────────────────────────────────────────────────
    CROW_ROUTE(app, "/api/login").methods("POST"_method)([](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");

        std::string email    = body["email"].s();
        std::string password = body["password"].s();

        auto users = readBinary<User>(USERS_FILE);
        for (const auto& u : users) {
            if (email == u.email && password == u.password) {
                crow::json::wvalue res;
                res["id"]             = u.id;
                res["name"]           = std::string(u.name);
                res["email"]          = std::string(u.email);
                res["age"]            = u.age;
                res["height"]         = u.height;
                res["weight"]         = u.weight;
                res["gender"]         = std::string(u.gender);
                res["activity_level"] = std::string(u.activity_level);
                res["goal"]           = std::string(u.goal);
                return crow::response(200, res);
            }
        }
        return crow::response(401, "Unauthorized");
    });

    // ── /api/journal (GET) ──────────────────────────────────────────────────
    CROW_ROUTE(app, "/api/journal").methods("GET"_method)([](const crow::request& req) {
        char* date_cstr = req.url_params.get("date");
        char* uid_cstr  = req.url_params.get("user_id");
        if (!date_cstr || !uid_cstr) return crow::response(400, "Missing parameters");

        std::string date = date_cstr;
        int user_id = std::stoi(uid_cstr);

        auto products = readBinary<Product>(PRODUCTS_FILE);
        std::vector<crow::json::wvalue> items;

        for (const auto& p : products) {
            if (p.user_id == user_id && date == p.date) {
                crow::json::wvalue item;
                item["id"]       = p.id;
                item["name"]     = std::string(p.name);
                item["weight"]   = p.weight;
                item["calories"] = p.calories;
                item["category"] = std::string(p.category);
                item["time"]     = std::string(p.time);
                items.push_back(std::move(item));
            }
        }
        return crow::response(200, crow::json::wvalue(items));
    });

    // ── /api/journal/add (POST) ─────────────────────────────────────────────
    CROW_ROUTE(app, "/api/journal/add").methods("POST"_method)([](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");

        auto products = readBinary<Product>(PRODUCTS_FILE);
        Product p{};
        p.id      = products.empty() ? 1 : products.back().id + 1;
        p.user_id = body["user_id"].i();
        copyStr(p.name,     body["name"].s(),     sizeof(p.name));
        p.weight   = body["weight"].d();
        p.calories = body["calories"].d();
        copyStr(p.category, body["category"].s(), sizeof(p.category));
        copyStr(p.time,     body["time"].s(),     sizeof(p.time));
        copyStr(p.date,     body["date"].s(),     sizeof(p.date));

        appendBinary(PRODUCTS_FILE, p);
        return crow::response(200, "OK");
    });

    // ── /api/profile (GET) ──────────────────────────────────────────────────
    CROW_ROUTE(app, "/api/profile").methods("GET"_method)([](const crow::request& req) {
        char* uid_cstr = req.url_params.get("user_id");
        if (!uid_cstr) return crow::response(400, "Missing user_id");
        int user_id = std::stoi(uid_cstr);

        auto users = readBinary<User>(USERS_FILE);
        User* found_user = nullptr;
        for (auto& u : users) {
            if (u.id == user_id) { found_user = &u; break; }
        }
        if (!found_user) return crow::response(404, "User not found");

        Gender g = (std::string(found_user->gender) == "female") ? Gender::Female : Gender::Male;

        ActivityLevel a = ActivityLevel::Sedentary;
        std::string act(found_user->activity_level);
        if      (act == "light")    a = ActivityLevel::Light;
        else if (act == "moderate") a = ActivityLevel::Moderate;
        else if (act == "active")   a = ActivityLevel::Active;

        Goal gl = Goal::MaintainWeight;
        std::string goal_str(found_user->goal);
        if      (goal_str == "lose") gl = Goal::LoseWeight;
        else if (goal_str == "gain") gl = Goal::GainWeight;

        int limit = CalorieCalculator::getRecommendedCalories(
            g, found_user->weight, found_user->height, found_user->age, a, gl);

        auto t  = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d");
        std::string today = oss.str();

        double consumed = 0;
        auto products = readBinary<Product>(PRODUCTS_FILE);
        for (const auto& p : products) {
            if (p.user_id == user_id && std::string(p.date) == today)
                consumed += p.calories;
        }

        crow::json::wvalue res;
        res["name"]          = std::string(found_user->name);
        res["age"]           = found_user->age;
        res["gender"]        = std::string(found_user->gender);
        res["height"]        = found_user->height;
        res["weight"]        = found_user->weight;
        res["goal"]          = std::string(found_user->goal);
        res["dailyLimit"]    = limit;
        res["consumedToday"] = consumed;
        return crow::response(200, res);
    });

    // ── /api/catalog (GET) — НОВИЙ ──────────────────────────────────────────
    // Повертає всі продукти бази; фільтрує за ?query=назва (необов'язково)
    CROW_ROUTE(app, "/api/catalog").methods("GET"_method)([](const crow::request& req) {
        struct CatalogItem {
            int id;
            const char* name;
            int caloriesPer100g;
            double protein;
            double fat;
            double carbs;
            const char* category;
        };

        static const CatalogItem catalog[] = {
            {1,  "Яблуко",               52,  0.3,  0.2,  14.0, "Фрукти"},
            {2,  "Банан",                96,  1.3,  0.3,  27.0, "Фрукти"},
            {12, "Авокадо",             160,  2.0, 15.0,   9.0, "Фрукти"},
            {16, "Апельсин",             47,  0.9,  0.1,  12.0, "Фрукти"},
            {17, "Полуниця",             32,  0.7,  0.3,   7.7, "Фрукти"},
            {3,  "Куряча грудка",       165, 31.0,  3.6,   0.0, "Білкові"},
            {7,  "Яйце",               155, 13.0, 11.0,   1.1, "Білкові"},
            {13, "Лосось",             206, 22.0, 12.0,   0.0, "Білкові"},
            {15, "Тофу",               145, 16.0,  9.0,   2.0, "Білкові"},
            {19, "Яловичина (фарш)",   217, 26.0, 11.0,   0.0, "Білкові"},
            {4,  "Рис (варений)",       130,  2.7,  0.3,  28.0, "Зернові"},
            {8,  "Вівсянка (суха)",     389, 16.9,  6.9,  66.3, "Зернові"},
            {9,  "Цільнозерновий хліб", 265, 13.0,  3.4,  48.0, "Зернові"},
            {18, "Макарони (варені)",   131,  5.0,  1.0,  25.0, "Зернові"},
            {5,  "Грецький йогурт",      59, 10.0,  0.4,   3.6, "Молочні"},
            {10, "Молоко",               61,  3.2,  3.3,   4.8, "Молочні"},
            {14, "Сир Чеддер",          404, 23.0, 33.0,   3.1, "Молочні"},
            {6,  "Мигдаль",             579, 21.2, 49.9,  21.6, "Горіхи та жири"},
            {11, "Оливкова олія",        884,  0.0,100.0,   0.0, "Горіхи та жири"},
        };

        std::string query;
        if (auto q = req.url_params.get("query")) {
            query = q;
            std::transform(query.begin(), query.end(), query.begin(), ::tolower);
        }

        std::vector<crow::json::wvalue> results;
        for (const auto& item : catalog) {
            std::string nameLower = item.name;
            std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);

            if (query.empty() || nameLower.find(query) != std::string::npos) {
                crow::json::wvalue w;
                w["id"]              = item.id;
                w["name"]            = item.name;
                w["caloriesPer100g"] = item.caloriesPer100g;
                w["protein"]         = item.protein;
                w["fat"]             = item.fat;
                w["carbs"]           = item.carbs;
                w["category"]        = item.category;
                results.push_back(std::move(w));
            }
        }
        return crow::response(200, crow::json::wvalue(results));
    });

    // ── /api/stats (GET) — НОВИЙ ────────────────────────────────────────────
    // Повертає калорії за кожен день для юзера (для графіка статистики)
    CROW_ROUTE(app, "/api/stats").methods("GET"_method)([](const crow::request& req) {
        char* uid_cstr = req.url_params.get("user_id");
        if (!uid_cstr) return crow::response(400, "Missing user_id");
        int user_id = std::stoi(uid_cstr);

        auto products = readBinary<Product>(PRODUCTS_FILE);
        std::map<std::string, double> dayMap;
        for (const auto& p : products) {
            if (p.user_id == user_id)
                dayMap[std::string(p.date)] += p.calories;
        }

        std::vector<crow::json::wvalue> days;
        for (auto& kv : dayMap) {
            crow::json::wvalue entry;
            entry["date"]     = kv.first;
            entry["calories"] = kv.second;
            days.push_back(std::move(entry));
        }
        return crow::response(200, crow::json::wvalue(days));
    });

    app.port(8080).multithreaded().run();
    return 0;
}
