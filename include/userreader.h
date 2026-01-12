#pragma once
#include <vector>
#include <utility>
#include "user.h"

class DBProcessing;

class UserReader {
public:
    UserReader();
    std::pair<bool, std::vector<User>> requestUserBrowse();
    ~UserReader();

private:
};
