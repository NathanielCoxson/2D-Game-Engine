#pragma once

#include <string>

class Action {
    std::string name;
    std::string type;

public:
    Action(const std::string& n, const std::string& t): name(n), type(t) {};

    const std::string& getName() const {
        return name; 
    }

    const std::string& getType() const {
        return type;
    }
};
