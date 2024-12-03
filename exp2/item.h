// item.h
#ifndef ITEM_H_
#define ITEM_H_

#include "production.h"
#include "symbol.h"

class Item {
private:
    std::shared_ptr<Production> production_;
    size_t dot_position_;
    std::shared_ptr<Symbol> lookahead_;

public:
    Item(std::shared_ptr<Production> production, size_t dot_position, std::shared_ptr<Symbol> lookahead)
        : production_(production), dot_position_(dot_position), lookahead_(lookahead) {}

    bool IsReduceItem() const { return dot_position_ == production_->GetBody().size(); }
    std::shared_ptr<Production> GetProduction() const { return production_; }
    size_t GetDotPosition() const { return dot_position_; }
    std::shared_ptr<Symbol> GetLookahead() const { return lookahead_; }
};

#endif  // ITEM_H_