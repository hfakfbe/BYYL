// item_set_collection.h

#ifndef ITEM_SET_COLLECTION_H_
#define ITEM_SET_COLLECTION_H_

#include <set>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>
#include <iostream>
#include <cassert>
#include "item_set.h"
#include "grammar.h"
#include "lr1_table.h"

class ItemSetCollection {
private:
    std::vector<ItemSet> item_sets_;
    Grammar grammar_;
    // Use it to build LR(1) analysis table: goto and action
    std::map<std::pair<size_t, Symbol>, size_t> go_;

    // Create const symbols
    const Symbol start_symbol;
    const Symbol end_symbol;
    const Symbol epsilon_symbol;
    // First(A), A can be both terminal and nonterminal symbol
    std::map<Symbol, std::set<Symbol>> first_;

    // find next symbols w.r.t. item set
    std::set<Symbol> FindNextSymbols_(const ItemSet &item_set) {
        std::set<Symbol> symbols;
        for(const auto &item : item_set.items) {
            if(item.production.right.size() > item.dot_position) {
                symbols.insert(item.production.right[item.dot_position]);
            }
        }
        return symbols;
    }

    // find first
    // input Y1Y2...Yk
    // output FIRST(Y1Y2...Yk)
    std::set<Symbol> GetFirst4Candidate_(std::vector<Symbol> right){
        if(right.size() == 0) {
            return {epsilon_symbol};
        }
        std::set<Symbol> ret = first_[right[0]];
        ret.erase(epsilon_symbol);
        int is_eps = 1;
        for(int i = 1; i < right.size(); i++) {
            if(first_[right[i - 1]].count(epsilon_symbol)) {
                ret.insert(first_[right[i]].begin(), first_[right[i]].end());
                ret.erase(epsilon_symbol);
            }else{
                is_eps = 0;
                break;
            }
        }
        if(is_eps && first_[right.back()].count(epsilon_symbol)) {
            ret.insert(epsilon_symbol);
        }
        return ret;
    }

    // find first of all nonterminals from grammar 
    void FindFirst_(){
        for(auto &nonterminal : grammar_.GetNonTerminals()) {
            first_[nonterminal] = {};
        }
        for(auto &terminal : grammar_.GetTerminals()) {
            first_[terminal] = {terminal};
        }
        int inc = 1;
        while(inc){
            inc = 0;
            for(auto &production : grammar_.GetProductions()) {
                std::set<Symbol> first_right = GetFirst4Candidate_(production.right);
                for(auto &symbol : first_right) {
                    if(first_[production.left].count(symbol) == 0) {
                        first_[production.left].insert(symbol);
                        inc = 1;
                    }
                }
            }
        }
    }

    // find next item, actually compute closure
    // input item
    // output closure({item})
    std::set<Item> Closure_(const Item &item_) {
        std::set<Item> items;
        items.insert(item_);
        int inc = 1;
        while(inc){
            inc = 0;
            for(const auto &item : items) {
                if(item.production.right.size() > item.dot_position) {
                    Symbol next_symbol = item.production.right[item.dot_position];
                    if(next_symbol.type == Symbol::Type::NONTERMINAL) {
                        // Find first
                        auto right = item.production.right;
                        right.push_back(item.lookahead);
                        std::set<Symbol> lookaheads = GetFirst4Candidate_(std::vector<Symbol>(right.begin() + item.dot_position + 1, right.end()));
                        for(const auto &production : grammar_.GetProductions()) {
                            if(production.left == next_symbol) {
                                for(const auto &lookahead : lookaheads) {
                                    auto t = Item(production, 0, lookahead);
                                    if(!items.count(t)) {
                                        items.insert(t);
                                        inc = 1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return items;
    }
public:
    ItemSetCollection(const Grammar &grammar, int skip = 0)
        : grammar_(grammar), 
        start_symbol("S'", Symbol::Type::NONTERMINAL), 
        end_symbol("#", Symbol::Type::TERMINAL), 
        epsilon_symbol("eps", Symbol::Type::TERMINAL) {
        // Augmented grammar
        Production initial_production(start_symbol, {grammar.GetStartSymbol()});
        auto augprods = grammar.GetProductions();
        augprods.push_back(initial_production);
        auto augnont = grammar.GetNonTerminals();
        augnont.insert(start_symbol);
        auto augterm = grammar.GetTerminals();
        augterm.insert(end_symbol);
        grammar_ = Grammar(start_symbol, augterm, augnont, augprods);
        // Skip the following steps if skip is set
        if(skip){
            return;
        }
        // Initialize First(A) for all non-terminals A
        FindFirst_();
        // Create the initial item set
        auto initial_item = Item(initial_production, 0, end_symbol);
        ItemSet initial_item_set(Closure_(initial_item));
        item_sets_.push_back(initial_item_set);
        // Create the item sets
        for(int i = 0; i < item_sets_.size(); i++) {
            std::set<Symbol> symbols = FindNextSymbols_(item_sets_[i]);
            // For each symbol
            for(const auto &symbol : symbols) {
                std::set<Item> new_items;
                // For each item
                for(const auto &item : item_sets_[i].items) {
                    if(item.production.right.size() > item.dot_position && item.production.right[item.dot_position] == symbol) {
                        std::set<Item> next_items = Closure_(Item(item.production, item.dot_position + 1, item.lookahead));
                        new_items.insert(next_items.begin(), next_items.end());
                    }
                }
                assert(new_items.size() > 0);
                ItemSet new_item_set(new_items);
                auto it = std::find(item_sets_.begin(), item_sets_.end(), new_item_set);
                if(it == item_sets_.end()) {
                    item_sets_.push_back(new_item_set);
                    go_[{i, symbol}] = item_sets_.size() - 1;
                }else{
                    go_[{i, symbol}] = it - item_sets_.begin();
                }
            }
        }
    }
    
    void OutLR1Table(std::ostream &out) {
        std::map<std::pair<size_t, Symbol>, std::set<std::string>> out_table;
        for(auto [key, value] : go_) {
            if(key.second.type == Symbol::Type::TERMINAL) {
                out_table[key].insert("s" + std::to_string(value));
            }else{
                out_table[key].insert(std::to_string(value));
            }
        }
        for(int i = 0; i < item_sets_.size(); i ++){
            for(const auto &item : item_sets_[i].items) {
                if(item.production.right.size() == item.dot_position) {
                    if(item.production.left == start_symbol) {
                        out_table[{i, end_symbol}].insert("acc");
                    }else{
                        auto it = std::find(grammar_.GetProductions().begin(), grammar_.GetProductions().end(), item.production);
                        assert(it != grammar_.GetProductions().end());
                        out_table[{i, item.lookahead}].insert("r" + std::to_string(it - grammar_.GetProductions().begin()));
                    }
                }
            }
        } 

        out << "id";
        for(const auto &symbol : grammar_.GetTerminals()) {
            out << ", ";
            // walkaround
            if(symbol.name == ","){
                out << "comma";
            }else{
                out << symbol.name;
            }
        }
        for(const auto &symbol : grammar_.GetNonTerminals()) {
            out << ", " << symbol.name;
        }
        out << std::endl;
        for(int i = 0; i < item_sets_.size(); i++) {
            out << i;
            for(const auto &symbol : grammar_.GetTerminals()) {
                out << ", ";
                if(out_table.count({i, symbol})) {
                    for(auto &s : out_table[{i, symbol}]) {
                        out << s << " ";
                    }
                }
            }
            for(const auto &symbol : grammar_.GetNonTerminals()) {
                out << ", ";
                if(out_table.count({i, symbol})) {
                    for(auto &s : out_table[{i, symbol}]) {
                        out << s << " ";
                    }
                }
            }
            out << std::endl;
        }
    }
    
    void OutItems(std::ostream &out) {
        auto item_to_string = [](const Item &item) {
            std::string right = "";
            for(int i = 0; i < item.production.right.size(); i++) {
                if(i == item.dot_position) {
                    right += ".";
                }
                right += item.production.right[i].name;
            }
            if(item.dot_position == item.production.right.size()) {
                right += ".";
            }
            return item.production.left.name + " -> " + right + ", " + item.lookahead.name;
        };
        for(int i = 0; i < item_sets_.size(); i++) {
            out << "Item set " << i << std::endl;
            for(const auto &item : item_sets_[i].items) {
                out << item_to_string(item) << std::endl;
            }
            out << std::endl;
        }
    }

    const std::vector<ItemSet> &GetItemSets() const {
        return item_sets_;
    }

    const Grammar &GetGrammar() const {
        return grammar_;
    }
};

#endif  // ITEM_SET_COLLECTION_H_