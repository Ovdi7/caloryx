#pragma once

#include <string>
#include <vector>

#include "../Models/AccountData.h"

class AccountStorage {
private:
    std::string filePath;

    std::vector<AccountData> loadRaw() const;
    void saveRaw(const std::vector<AccountData>& accounts) const;

public:
    explicit AccountStorage(const std::string& filePath);

    bool fileExists() const;
    void initializeEmpty() const;

    void add(const AccountData& account) const;
    std::vector<AccountData> getAll() const;
    bool findById(int id, AccountData& result) const;
    bool findByLogin(const std::string& login, AccountData& result) const;
    bool update(const AccountData& account) const;
    bool remove(int id) const;
};
