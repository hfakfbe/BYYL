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

    bool operator==(const Production &other) const {
        return left == other.left && right == other.right;
    }
    bool operator<(const Production &other) const {
        if(left == other.left) {
            return right < other.right;
        }
        return left < other.left;
    }
};

#endif  // PRODUCTION_H_