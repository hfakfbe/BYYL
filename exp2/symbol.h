// symbol.h
#ifndef SYMBOL_H_
#define SYMBOL_H_

#include <string>

class Symbol {
public:
    std::string name;
    enum class Type { NONTERMINAL, TERMINAL } type;

    Symbol(const std::string& name, Type type)
        : name(name), type(type) {}
    bool operator==(const Symbol& other) const {
        return name == other.name && type == other.type;
    }
};

#endif  // SYMBOL_H_
