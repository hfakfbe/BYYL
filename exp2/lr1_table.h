// lr1_table.h
#ifndef LR1_TABLE_H_
#define LR1_TABLE_H_

#include <map>
#include "grammar.h"
#include <functional>

class LR1Table {
private:
    std::map<Symbol, size_t> goto_;
    std::map<Symbol, std::pair<char, size_t>> action_;
    Grammar grammar_;

public:
    LR1Table(const std::map<Symbol, size_t> &goto_table,
             const std::map<Symbol, std::pair<char, size_t>> &action_table,
             const Grammar &grammar)
        : goto_(goto_table), action_(action_table), grammar_(grammar) {}

    
};

#endif  // LR1_TABLE_H_