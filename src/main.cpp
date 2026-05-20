#include "crow_all.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <ctime>
#include <iomanip>
#include "Models/CalorieCalculator.h"

const std::string USERS_FILE = "users.dat";
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

    CROW_ROUTE(app, "/api/register").methods("POST"_method)([](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");
        
        auto users = readBinary<User>(USERS_FILE);
        User u{};
        u.id = users.empty() ? 1 : users.back().id + 1;
        copyStr(u.name, body["name"].s(), sizeof(u.name));
        copyStr(u.email, body["email"].s(), sizeof(u.email));
        copyStr(u.password, body["password"].s(), sizeof(u.password));
        copyStr(u.gender, body["gender"].s(), sizeof(u.gender));
        u.age = body["age"].i();
        u.height = body["height"].d();
        u.weight = body["weight"].d();
        copyStr(u.activity_level, body["activity_level"].s(), sizeof(u.activity_level));
        copyStr(u.goal, body["goal"].s(), sizeof(u.goal));
        
        appendBinary(USERS_FILE, u);
        
        crow::json::wvalue res;
        res["id"] = u.id;
        res["name"] = std::string(u.name);
        res["email"] = std::string(u.email);
        res["age"] = u.age;
        res["height"] = u.height;
        res["weight"] = u.weight;
        res["gender"] = std::string(u.gender);
        res["activity_level"] = std::string(u.activity_level);
        res["goal"] = std::string(u.goal);
        return crow::response(200, res);
    });

    CROW_ROUTE(app, "/api/login").methods("POST"_method)([](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");
        
        std::string email = body["email"].s();
        std::string password = body["password"].s();
        
        auto users = readBinary<User>(USERS_FILE);
        for (const auto& u : users) {
            if (email == u.email && password == u.password) {
                crow::json::wvalue res;
                res["id"] = u.id;
                res["name"] = std::string(u.name);
                res["email"] = std::string(u.email);
                res["age"] = u.age;
                res["height"] = u.height;
                res["weight"] = u.weight;
                res["gender"] = std::string(u.gender);
                res["activity_level"] = std::string(u.activity_level);
                res["goal"] = std::string(u.goal);
                return crow::response(200, res);
            }
        }
        return crow::response(401, "Unauthorized");
    });

    CROW_ROUTE(app, "/api/journal").methods("GET"_method)([](const crow::request& req) {
        char* date_cstr = req.url_params.get("date");
        char* uid_cstr = req.url_params.get("user_id");
        if (!date_cstr || !uid_cstr) return crow::response(400, "Missing parameters");
        
        std::string date = date_cstr;
        int user_id = std::stoi(uid_cstr);
        
        auto products = readBinary<Product>(PRODUCTS_FILE);
        std::vector<crow::json::wvalue> items;
        
        for (const auto& p : products) {
            if (p.user_id == user_id && date == p.date) {
                crow::json::wvalue item;
                item["id"] = p.id;
                item["name"] = std::string(p.name);
                item["weight"] = p.weight;
                item["calories"] = p.calories;
                item["category"] = std::string(p.category);
                item["time"] = std::string(p.time);
                items.push_back(std::move(item));
            }
        }
        return crow::response(200, crow::json::wvalue(items));
    });

    CROW_ROUTE(app, "/api/journal/add").methods("POST"_method)([](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");
        
        auto products = readBinary<Product>(PRODUCTS_FILE);
        Product p{};
        p.id = products.empty() ? 1 : products.back().id + 1;
        p.user_id = body["user_id"].i();
        copyStr(p.name, body["name"].s(), sizeof(p.name));
        p.weight = body["weight"].d();
        p.calories = body["calories"].d();
        copyStr(p.category, body["category"].s(), sizeof(p.category));
        copyStr(p.time, body["time"].s(), sizeof(p.time));
        copyStr(p.date, body["date"].s(), sizeof(p.date));
        
        appendBinary(PRODUCTS_FILE, p);
        return crow::response(200, "OK");
    });

    CROW_ROUTE(app, "/api/profile").methods("GET"_method)([](const crow::request& req) {
        char* uid_cstr = req.url_params.get("user_id");
        if (!uid_cstr) return crow::response(400, "Missing user_id");
        int user_id = std::stoi(uid_cstr);
        
        auto users = readBinary<User>(USERS_FILE);
        User* found_user = nullptr;
        for (auto& u : users) {
            if (u.id == user_id) {
                found_user = &u;
                break;
            }
        }
        if (!found_user) return crow::response(404, "User not found");
        
        Gender g = (std::string(found_user->gender) == "female") ? Gender::Female : Gender::Male;
        
        ActivityLevel a = ActivityLevel::Sedentary;
        std::string act(found_user->activity_level);
        if (act == "light") a = ActivityLevel::Light;
        else if (act == "moderate") a = ActivityLevel::Moderate;
        else if (act == "active") a = ActivityLevel::Active;
        
        Goal gl = Goal::MaintainWeight;
        std::string goal_str(found_user->goal);
        if (goal_str == "lose") gl = Goal::LoseWeight;
        else if (goal_str == "gain") gl = Goal::GainWeight;
        
        int limit = CalorieCalculator::getRecommendedCalories(g, found_user->weight, found_user->height, found_user->age, a, gl);
        
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d");
        std::string today = oss.str();
        
        double consumed = 0;
        auto products = readBinary<Product>(PRODUCTS_FILE);
        for (const auto& p : products) {
            if (p.user_id == user_id && std::string(p.date) == today) {
                consumed += p.calories;
            }
        }
        
        crow::json::wvalue res;
        res["name"] = std::string(found_user->name); res["age"] = found_user->age;
        res["gender"] = std::string(found_user->gender); res["height"] = found_user->height;
        res["weight"] = found_user->weight; res["goal"] = std::string(found_user->goal);
        res["dailyLimit"] = limit; res["consumedToday"] = consumed;
        return crow::response(200, res);
    });

    app.port(8080).multithreaded().run();
    return 0;
}
