// item.h
#ifndef ITEM_H_
#define ITEM_H_

#include "production.h"
#include "symbol.h"

class Item {
public:
    std::shared_ptr<Production> production;
    size_t dot_position;
    std::shared_ptr<Symbol> lookahead;

    Item(std::shared_ptr<Production> production, size_t dot_position, std::shared_ptr<Symbol> lookahead)
        : production(production), dot_position(dot_position), lookahead(lookahead) {}
};

#endif  // ITEM_H_