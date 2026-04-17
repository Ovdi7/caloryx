#pragma once

#include <string>
#include <vector>

#include "../Models/AccountData.h"
#include "../Storage/AccountStorage.h"

class AuthService {
private:
    AccountStorage& storage;

    int generateNextId() const;

public:
    explicit AuthService(AccountStorage& storage);

    void ensureSeedData() const;
    std::vector<AccountData> getAllAccounts() const;
    AccountData registerUser(
        const std::string& login,
        const std::string& password,
        const std::string& displayName,
        int age,
        double weight,
        double height,
        int dailyCalories
    ) const;
    AccountData login(const std::string& login, const std::string& password) const;
    AccountData getAccountById(int id) const;

    static std::string hashPassword(const std::string& password);
};
