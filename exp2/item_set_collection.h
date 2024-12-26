// item_set_collection.h

#ifndef ITEM_SET_COLLECTION_H_
#define ITEM_SET_COLLECTION_H_

#include <set>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>
#include "item_set.h"
#include "grammar.h"

class ItemSetCollection {
private:
    std::vector<ItemSet> item_sets_;
    std::map<std::pair<size_t, Symbol>, size_t> goto_table_;
    Grammar grammar_;

    // Create const symbols
    const Symbol start_symbol;
    const Symbol end_symbol;
    const Symbol epsilon_symbol;

    // find next symbols
    std::set<Symbol> FindNextSymbols_(const ItemSet &item_set) {
        std::set<Symbol> symbols;
        for(const auto &item : item_set.items) {
            if(item.production.right.size() > item.dot_position) {
                symbols.insert(item.production.right[item.dot_position]);
            }
        }
        return symbols;
    }

    // find first of all nonterminals from grammar 
    void FindFirst_(){
        static std::map<Symbol, std::set<Symbol>> first;

    }

    // find first for candidate


    // find first
    std::set<Symbol> FindFirstSymbols_(const std::vector<Symbol> &symbols) {
        std::set<Symbol> first;
        for(const auto &symbol : symbols) {
            if(symbol.type == Symbol::Type::TERMINAL) {
                first.insert(symbol);
                return first;
            }else if(symbol.type == Symbol::Type::NONTERMINAL) {
                auto nfirst = FindFirst_(symbol);
                int eps = 0;
                if(nfirst.count(epsilon_symbol)) {
                    eps = 1;
                    nfirst.erase(epsilon_symbol);
                }
                first.insert(nfirst.begin(), nfirst.end());
                if(eps == 0) {
                    return first;
                }   
            }
        }
    }

    // find next item, actually compute closure
    std::set<Item> FindNextItems_(const Item &item) {
        std::set<Item> items;
        if(item.production.right.size() > item.dot_position) {
            Symbol next_symbol = item.production.right[item.dot_position];
            if(next_symbol.type == Symbol::Type::NONTERMINAL) {
                // Find first
                auto right = item.production.right;
                right.push_back(item.lookahead);
                std::set<Symbol> lookaheads = FindFirst_(std::vector<Symbol>(right.begin() + item.dot_position + 1, right.end()));
                for(const auto &production : grammar_.GetProductions()) {
                    if(production.left == next_symbol) {
                        for(const auto &lookahead : lookaheads) {
                            items.insert(Item(production, 0, lookahead));
                        }
                    }
                }
            }
        }
        return items;
    }
public:
    ItemSetCollection(const Grammar &grammar)
        : grammar_(grammar), 
        start_symbol("S'", Symbol::Type::NONTERMINAL), 
        end_symbol("#", Symbol::Type::TERMINAL), 
        epsilon_symbol("eps", Symbol::Type::TERMINAL) {
        // Create the initial item set
        Production initial_production(start_symbol, {grammar.GetStartSymbol()});
        ItemSet initial_item_set({Item(initial_production, 0, end_symbol)});
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
                        std::set<Item> next_items = FindNextItems_(Item(item.production, item.dot_position + 1, item.lookahead));
                        new_items.insert(next_items.begin(), next_items.end());
                    }
                }
                if(new_items.size() == 0) {
                    continue;
                }
                ItemSet new_item_set(new_items);
                auto it = std::find(item_sets_.begin(), item_sets_.end(), new_item_set);
                if(it == item_sets_.end()) {
                    item_sets_.push_back(new_item_set);
                }
                goto_table_[{i, symbol}] = it - item_sets_.begin();
            }
        }
    }
        
};

#endif  // ITEM_SET_COLLECTION_H_