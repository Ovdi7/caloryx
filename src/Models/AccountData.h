#pragma once

struct AccountData {
    static const int LoginSize = 32;
    static const int PasswordHashSize = 32;
    static const int DisplayNameSize = 64;

    int id;
    char login[LoginSize];
    char passwordHash[PasswordHashSize];
    int role;
    char displayName[DisplayNameSize];
    int birthYear;
    int birthMonth;
    int birthDay;
    double weight;
    double height;
    int dailyCalories;
    int isDeleted;
};
