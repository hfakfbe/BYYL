// item.h
#ifndef ITEM_H_
#define ITEM_H_

#include "production.h"
#include "symbol.h"

class Item {
public:
    Production production;
    size_t dot_position;
    Symbol lookahead; // search symbol

    Item(Production production, size_t dot_position, Symbol lookahead)
        : production(production), dot_position(dot_position), lookahead(lookahead) {}
    
    bool operator==(const Item& other) const {
        return production == other.production && dot_position == other.dot_position && lookahead == other.lookahead;
    }
    bool operator<(const Item& other) const {
        if(production == other.production) {
            if(dot_position == other.dot_position) {
                return lookahead < other.lookahead;
            }
            return dot_position < other.dot_position;
        }
        return production < other.production;
    }
};

#endif  // ITEM_H_