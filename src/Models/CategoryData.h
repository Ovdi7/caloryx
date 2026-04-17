#pragma once

struct CategoryData {
    static const int NameSize = 48;

    int id;
    char name[NameSize];
    int isDeleted;
};
