// item_set.h
#ifndef ITEM_SET_H_
#define ITEM_SET_H_

#include <set>
#include "item.h"

class ItemSet {
public:
    std::set<Item> items;

    bool operator==(const ItemSet& other) const { 
        return items == other.items; 
    }
    ItemSet(const std::set<Item>& items) : items(items) {}
};

#endif  // ITEM_SET_H_