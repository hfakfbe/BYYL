// production.h
#ifndef PRODUCTION_H_
#define PRODUCTION_H_

#include <memory>
#include <vector>
#include <set>
#include "symbol.h"

class Production {
public:
    Symbol left;
    std::vector<Symbol> right;

    Production(const Symbol &left, const std::vector<Symbol> &right)
        : left(left), right(right) {}
};

#endif  // PRODUCTION_H_