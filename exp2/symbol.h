// symbol.h
#ifndef SYMBOL_H_
#define SYMBOL_H_

#include <string>

class Symbol {
public:
    std::string name;
    bool is_terminal;

    Symbol(const std::string& name, bool is_terminal)
        : name(name), is_terminal(is_terminal) {}
};

#endif  // SYMBOL_H_
