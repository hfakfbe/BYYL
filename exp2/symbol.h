// symbol.h
#ifndef SYMBOL_H_
#define SYMBOL_H_

#include <string>

class Symbol {
public:
    std::string name, value;
    enum class Type { NONTERMINAL, TERMINAL } type;

    Symbol(const std::string& name, Type type)
        : name(name), type(type) {}
    bool operator==(const Symbol& other) const {
        return name == other.name && type == other.type;
    }
    bool operator<(const Symbol& other) const {
        if(name == other.name) {
            return type < other.type;
        }
        return name < other.name;
    }
};

#endif  // SYMBOL_H_
