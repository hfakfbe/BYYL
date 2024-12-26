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
};

#endif  // ITEM_H_