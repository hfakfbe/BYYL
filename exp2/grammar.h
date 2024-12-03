// grammar.h
#ifndef GRAMMAR_H_
#define GRAMMAR_H_

#include <memory>
#include <vector>
#include <set>
#include "production.h"
#include "symbol.h"

class Grammar {
private:
    std::vector<std::shared_ptr<Production>> productions_;
    std::shared_ptr<Symbol> start_symbol_;

public:
    Grammar(std::shared_ptr<Symbol> start_symbol) : start_symbol_(start_symbol) {}

    void AddProduction(std::shared_ptr<Production> production) { productions_.push_back(production); }
    std::shared_ptr<Symbol> GetStartSymbol() const { return start_symbol_; }
    const std::vector<std::shared_ptr<Production>>& GetProductions() const { return productions_; }
    std::set<std::shared_ptr<Symbol>> GetNonTerminals() const;
    std::set<std::shared_ptr<Symbol>> GetTerminals() const;
    void ComputeFollowSets();
};

#endif  // GRAMMAR_H_