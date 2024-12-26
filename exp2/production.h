// production.h
#ifndef PRODUCTION_H_
#define PRODUCTION_H_

#include <memory>
#include <vector>
#include <set>
#include "symbol.h"

class Production {
public:
    std::shared_ptr<Symbol> left;
    std::vector<std::shared_ptr<Symbol>> right;

    Production(std::shared_ptr<Symbol> left, const std::vector<std::shared_ptr<Symbol>>& right)
        : left(left), right(right) {}
};

#endif  // PRODUCTION_H_