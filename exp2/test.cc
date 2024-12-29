#include <iostream>
#include "item_set_collection.h"

int main(){
    Symbol S("S", Symbol::Type::NONTERMINAL);
    Symbol A("A", Symbol::Type::NONTERMINAL);
    Symbol B("B", Symbol::Type::NONTERMINAL);
    Symbol a("a", Symbol::Type::TERMINAL);
    Symbol b("b", Symbol::Type::TERMINAL);
    Production p1(S, {a, A, a, A, b});
    Production p2(S, {a, B, b});
    Production p3(A, {a});
    Production p4(B, {a});
    std::set<Symbol> non_terminals = {S, A, B};
    std::set<Symbol> terminals = {a, b};
    std::vector<Production> productions = {p1, p2, p3, p4};
    Grammar grammar(S, terminals, non_terminals, productions);
    ItemSetCollection item_set_collection(grammar);
    item_set_collection.GetLR1Table(std::cout);
}