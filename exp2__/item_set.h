// item_set.h
#ifndef ITEM_SET_H_
#define ITEM_SET_H_

#include <set>
#include "item.h"

class ItemSet {
private:
    std::set<Item> items_;

public:
    void Closure();
    ItemSet GotoFunction(const std::shared_ptr<Symbol>& symbol);
    bool Contains(const Item& item) const { return items_.find(item) != items_.end(); }
    const std::set<Item>& GetItems() const { return items_; }
    bool operator==(const ItemSet& other) const { return items_ == other.items_; }
    void AddItem(const Item& item) { items_.insert(item); }
};

#endif  // ITEM_SET_H_