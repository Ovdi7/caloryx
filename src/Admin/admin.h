#pragma once

#include <string>

#include "../Models/AccountData.h"

class Admin {
private:
    AccountData account;

public:
    Admin();
    explicit Admin(const AccountData& account);

    int getId() const;
    std::string getLogin() const;
    std::string getDisplayName() const;
};
