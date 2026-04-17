#include "admin.h"

#include "../Common/FixedText.h"

Admin::Admin() : account{} {
}

Admin::Admin(const AccountData& account) : account(account) {
}

int Admin::getId() const {
    return account.id;
}

std::string Admin::getLogin() const {
    return toString(account.login);
}

std::string Admin::getDisplayName() const {
    return toString(account.displayName);
}
