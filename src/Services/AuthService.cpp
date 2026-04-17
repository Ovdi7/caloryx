#include "AuthService.h"

#include <stdexcept>

#include "../Common/FixedText.h"
#include "../Models/AccountRole.h"

AuthService::AuthService(AccountStorage& storage) : storage(storage) {
}

void AuthService::ensureSeedData() const {
    if (!storage.fileExists()) {
        storage.initializeEmpty();
    }

    if (!storage.getAll().empty()) {
        return;
    }

    AccountData admin{};
    admin.id = 1;
    copyText(admin.login, AccountData::LoginSize, "admin");
    copyText(admin.passwordHash, AccountData::PasswordHashSize, hashPassword("admin123"));
    admin.role = RoleAdmin;
    copyText(admin.displayName, AccountData::DisplayNameSize, "System Admin");
    admin.dailyCalories = 2000;
    admin.isDeleted = 0;
    storage.add(admin);

    AccountData demo{};
    demo.id = 2;
    copyText(demo.login, AccountData::LoginSize, "demo");
    copyText(demo.passwordHash, AccountData::PasswordHashSize, hashPassword("demo123"));
    demo.role = RoleUser;
    copyText(demo.displayName, AccountData::DisplayNameSize, "Demo User");
    demo.age = 23;
    demo.weight = 70.0;
    demo.height = 175.0;
    demo.dailyCalories = 2200;
    demo.isDeleted = 0;
    storage.add(demo);
}

std::vector<AccountData> AuthService::getAllAccounts() const {
    return storage.getAll();
}

AccountData AuthService::registerUser(
    const std::string& login,
    const std::string& password,
    const std::string& displayName,
    int age,
    double weight,
    double height,
    int dailyCalories
) const {
    if (login.empty() || password.empty() || displayName.empty()) {
        throw std::runtime_error("Login, password and display name are required.");
    }
    if (age <= 0 || weight <= 0.0 || height <= 0.0 || dailyCalories <= 0) {
        throw std::runtime_error("Profile values must be positive.");
    }

    AccountData existing{};
    if (storage.findByLogin(login, existing)) {
        throw std::runtime_error("Login already exists.");
    }

    AccountData account{};
    account.id = generateNextId();
    copyText(account.login, AccountData::LoginSize, login);
    copyText(account.passwordHash, AccountData::PasswordHashSize, hashPassword(password));
    account.role = RoleUser;
    copyText(account.displayName, AccountData::DisplayNameSize, displayName);
    account.age = age;
    account.weight = weight;
    account.height = height;
    account.dailyCalories = dailyCalories;
    account.isDeleted = 0;

    storage.add(account);
    return account;
}

AccountData AuthService::login(const std::string& login, const std::string& password) const {
    AccountData account{};
    if (!storage.findByLogin(login, account)) {
        throw std::runtime_error("User with this login does not exist.");
    }

    if (toString(account.passwordHash) != hashPassword(password)) {
        throw std::runtime_error("Invalid password.");
    }

    return account;
}

AccountData AuthService::getAccountById(int id) const {
    AccountData account{};
    if (!storage.findById(id, account)) {
        throw std::runtime_error("Account not found.");
    }
    return account;
}

std::string AuthService::hashPassword(const std::string& password) {
    const unsigned long long offsetBasis = 1469598103934665603ull;
    const unsigned long long prime = 1099511628211ull;

    unsigned long long hash = offsetBasis;
    for (unsigned char symbol : password) {
        hash ^= symbol;
        hash *= prime;
    }

    return std::to_string(hash);
}

int AuthService::generateNextId() const {
    int maxId = 0;
    for (const AccountData& account : storage.getAll()) {
        if (account.id > maxId) {
            maxId = account.id;
        }
    }

    return maxId + 1;
}
