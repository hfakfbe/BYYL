// lr1_table.h
#ifndef LR1_TABLE_H_
#define LR1_TABLE_H_

#include "grammar.h"
#include <map>
#include <functional>
#include <iostream>
#include <sstream>

class LR1Table {
private:
    std::map<std::pair<size_t, Symbol>, size_t> goto_;
    std::map<std::pair<size_t, Symbol>, std::pair<char, size_t>> action_;
    Grammar grammar_;

    Symbol start_symbol;
    Symbol end_symbol;

    std::vector<std::string> SplitCsv_(const std::string &line) {
        std::vector<std::string> result;
        std::string token;
        std::istringstream iss(line);
        while (std::getline(iss, token, ',')) {
            while(token.size() > 0 && token[0] == ' ') {
                token = token.substr(1);
            }
            while(token.size() > 0 && token[token.size() - 1] == ' ') {
                token = token.substr(0, token.size() - 1);
            }
            result.push_back(token);
        }
        return result;
    }

    std::vector<Symbol> SetHeader_(std::vector<std::string> &header) {
        int n = header.size() - 1;
        int cnt = grammar_.GetNonTerminals().size() + grammar_.GetTerminals().size();
        assert(n == cnt);
        assert(header[0] == "id");
        // read symbols
        std::vector<Symbol> symbols;
        symbols.push_back(Symbol("NA", Symbol::Type::TERMINAL));
        for(int i = 1; i <= n; i ++){
            auto sym = Symbol(header[i], Symbol::Type::TERMINAL);
            symbols.push_back(sym);
        }
        // check terminal
        for(auto &symbol : grammar_.GetTerminals()) {
            auto sym = Symbol(symbol.name, Symbol::Type::TERMINAL);
            auto it = std::find(symbols.begin(), symbols.end(), sym);
            assert(it != symbols.end());
        }
        // check non-terminal
        for(auto &symbol : grammar_.GetNonTerminals()) {
            auto sym = Symbol(symbol.name, Symbol::Type::TERMINAL);
            auto it = std::find(symbols.begin(), symbols.end(), sym);
            assert(it != symbols.end());
            symbols[it - symbols.begin()].type = Symbol::Type::NONTERMINAL;
        }
        return symbols;
    }

public:
    //read from csv file
    LR1Table(std::istream &in, const Grammar &grammar)
         : grammar_(grammar),
        start_symbol("S'", Symbol::Type::NONTERMINAL), 
        end_symbol("#", Symbol::Type::TERMINAL) {
        // first line is terminal symbols and non-terminal symbols
        std::string line;
        std::getline(in, line);
        auto header = SplitCsv_(line);
        auto symbols = SetHeader_(header);
        int n = header.size() - 1;
        // read table
        while(std::getline(in, line)) {
            auto items = SplitCsv_(line);
            assert(items.size() == n + 1);
            // first column is state number
            size_t i = std::stoi(items[0]);
            for(int j = 1; j <= n; j ++) {
                if(items[j].size() == 0) {
                    continue;
                }
                if(items[j][0] == 's') {
                    size_t k = std::stoi(items[j].substr(1));
                    action_[{i, symbols[j]}] = {'s', k};
                }else if(items[j][0] == 'r') {
                    size_t k = std::stoi(items[j].substr(1));
                    action_[{i, symbols[j]}] = {'r', k};
                }else if(items[j] == "acc") {
                    action_[{i, symbols[j]}] = {'a', 0};
                }else{
                    size_t k = std::stoi(items[j]);
                    goto_[{i, symbols[j]}] = k;
                }
            }
        }
    }
    
    const std::map<std::pair<size_t, Symbol>, size_t> &GetGoto() const {
        return goto_;
    }

    const std::map<std::pair<size_t, Symbol>, std::pair<char, size_t>> &GetAction() const {
        return action_;
    }

    const Grammar &GetGrammar() const {
        return grammar_;
    }

    const Symbol &GetStartSymbol() const {
        return start_symbol;
    }

    const Symbol &GetEndSymbol() const {
        return end_symbol;
    }
};

#endif  // LR1_TABLE_H_