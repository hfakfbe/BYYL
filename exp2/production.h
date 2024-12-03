// production.h
#ifndef PRODUCTION_H_
#define PRODUCTION_H_

#include <memory>
#include <vector>
#include <set>
#include "symbol.h"

class Production {
private:
    std::shared_ptr<Symbol> head_;
    std::vector<std::shared_ptr<Symbol>> body_;
    std::set<std::shared_ptr<Symbol>> follow_positions_;

public:
    Production(std::shared_ptr<Symbol> head, const std::vector<std::shared_ptr<Symbol>>& body)
        : head_(head), body_(body) {}

    void AddFollowPosition(const std::shared_ptr<Symbol>& symbol) { follow_positions_.insert(symbol); }
    std::shared_ptr<Symbol> GetHead() const { return head_; }
    const std::vector<std::shared_ptr<Symbol>>& GetBody() const { return body_; }
    const std::set<std::shared_ptr<Symbol>>& GetFollowPositions() const { return follow_positions_; }
};

#endif  // PRODUCTION_H_