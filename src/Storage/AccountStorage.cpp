#include "AccountStorage.h"

#include <fstream>
#include <stdexcept>

#include "../Common/FixedText.h"

AccountStorage::AccountStorage(const std::string& filePath) : filePath(filePath) {
}

bool AccountStorage::fileExists() const {
    std::ifstream input(filePath, std::ios::binary);
    return input.good();
}

void AccountStorage::initializeEmpty() const {
    std::ofstream output(filePath, std::ios::binary | std::ios::trunc);
    if (!output) {
        throw std::runtime_error("Cannot initialize account storage.");
    }
}

void AccountStorage::add(const AccountData& account) const {
    std::ofstream output(filePath, std::ios::binary | std::ios::app);
    if (!output) {
        throw std::runtime_error("Cannot open account storage for append.");
    }

    output.write(reinterpret_cast<const char*>(&account), sizeof(AccountData));
}

std::vector<AccountData> AccountStorage::getAll() const {
    std::vector<AccountData> result;
    for (const AccountData& account : loadRaw()) {
        if (!account.isDeleted) {
            result.push_back(account);
        }
    }
    return result;
}

bool AccountStorage::findById(int id, AccountData& result) const {
    for (const AccountData& account : loadRaw()) {
        if (!account.isDeleted && account.id == id) {
            result = account;
            return true;
        }
    }

    return false;
}

bool AccountStorage::findByLogin(const std::string& login, AccountData& result) const {
    for (const AccountData& account : loadRaw()) {
        if (!account.isDeleted && toString(account.login) == login) {
            result = account;
            return true;
        }
    }

    return false;
}

bool AccountStorage::update(const AccountData& account) const {
    std::vector<AccountData> accounts = loadRaw();
    for (AccountData& current : accounts) {
        if (!current.isDeleted && current.id == account.id) {
            current = account;
            saveRaw(accounts);
            return true;
        }
    }

    return false;
}

bool AccountStorage::remove(int id) const {
    std::vector<AccountData> accounts = loadRaw();
    for (AccountData& current : accounts) {
        if (!current.isDeleted && current.id == id) {
            current.isDeleted = 1;
            saveRaw(accounts);
            return true;
        }
    }

    return false;
}

std::vector<AccountData> AccountStorage::loadRaw() const {
    std::ifstream input(filePath, std::ios::binary);
    if (!input) {
        throw std::runtime_error("Cannot open account storage.");
    }

    std::vector<AccountData> result;
    AccountData record{};
    while (input.read(reinterpret_cast<char*>(&record), sizeof(AccountData))) {
        result.push_back(record);
    }

    if (!input.eof()) {
        throw std::runtime_error("Account storage is corrupted.");
    }

    return result;
}

void AccountStorage::saveRaw(const std::vector<AccountData>& accounts) const {
    std::ofstream output(filePath, std::ios::binary | std::ios::trunc);
    if (!output) {
        throw std::runtime_error("Cannot write account storage.");
    }

    for (const AccountData& account : accounts) {
        output.write(reinterpret_cast<const char*>(&account), sizeof(AccountData));
    }
}
