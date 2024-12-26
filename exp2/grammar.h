// grammar.h
#ifndef GRAMMAR_H_
#define GRAMMAR_H_

#include <memory>
#include <vector>
#include <set>
#include <stdexcept>
#include "production.h"
#include "symbol.h"

// 以下类是一个比较完整的上下文无关文法类，实现了基本功能。更进一步的功能可以从该类继承
class Grammar {
private:
    std::set<Symbol> terminals_;
    std::set<Symbol> non_terminals_;
    std::vector<Production> productions_;
    Symbol start_symbol_;

public:
    Symbol GetStartSymbol() const { 
        return start_symbol_; 
    }
    const std::vector<Production>& GetProductions() const { 
        return productions_; 
    }
    std::set<Symbol> GetNonTerminals() const {
        return non_terminals_;
    }
    std::set<Symbol> GetTerminals() const {
        return terminals_;
    }

    Grammar(Symbol start_symbol,
            std::set<Symbol> terminals,
            std::set<Symbol> non_terminals,
            std::vector<Production> productions)
        : start_symbol_(std::move(start_symbol)),
          terminals_(std::move(terminals)),
          non_terminals_(std::move(non_terminals)),
          productions_(std::move(productions)) {

        // Check if the start symbol is a non-terminal and is in the set of non-terminals
        if (start_symbol_.type != Symbol::Type::NONTERMINAL ||
            non_terminals_.find(start_symbol_) == non_terminals_.end()) {
            throw std::invalid_argument("Start symbol must be a non-terminal and defined in the set of non-terminals.");
        }

        // Validate each production
        for (const auto& production : productions_) {
            // Check if the left-hand side is a non-terminal and is in the set of non-terminals
            if (production.left.type != Symbol::Type::NONTERMINAL ||
                non_terminals_.find(production.left) == non_terminals_.end()) {
                throw std::invalid_argument("Production left-hand side must be a non-terminal and defined in the set of non-terminals.");
            }

            // Check all right-hand side symbols
            for (const auto& symbol : production.right) {
                bool isValid = false;
                if (symbol.type == Symbol::Type::TERMINAL && terminals_.find(symbol) != terminals_.end()) {
                    isValid = true;
                } else if (symbol.type == Symbol::Type::NONTERMINAL && non_terminals_.find(symbol) != non_terminals_.end()) {
                    isValid = true;
                }

                if (!isValid) {
                    throw std::invalid_argument("All production right-hand side symbols must be defined in the respective sets of terminals or non-terminals.");
                }
            }
        }
    }
};

#endif  // GRAMMAR_H_