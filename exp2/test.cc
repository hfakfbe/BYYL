#include <iostream>
#include "item_set_collection.h"

void test1(){
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

void test2(){
    Symbol S("S", Symbol::Type::NONTERMINAL);
    Symbol Q("Q", Symbol::Type::NONTERMINAL);
    Symbol o("o", Symbol::Type::TERMINAL);
    Symbol p("p", Symbol::Type::TERMINAL);
    Production p1(S, {Q, Q});
    Production p2(Q, {o, Q});
    Production p3(Q, {p});
    std::set<Symbol> non_terminals = {S, Q};
    std::set<Symbol> terminals = {o, p};
    std::vector<Production> productions = {p1, p2, p3};
    Grammar grammar(S, terminals, non_terminals, productions);
    ItemSetCollection item_set_collection(grammar);
    item_set_collection.GetLR1Table(std::cout);
}

void test3(){
    Symbol E("E", Symbol::Type::NONTERMINAL);
    Symbol plus("+", Symbol::Type::TERMINAL);
    Symbol mul("*", Symbol::Type::TERMINAL);
    Symbol lbr("(", Symbol::Type::TERMINAL);
    Symbol rbr(")", Symbol::Type::TERMINAL);
    Symbol i("i", Symbol::Type::TERMINAL);
    Production p1(E, {E, plus, E});
    Production p2(E, {E, mul, E});
    Production p3(E, {lbr, E, rbr});
    Production p4(E, {i});
    std::set<Symbol> non_terminals = {E};
    std::set<Symbol> terminals = {plus, mul, lbr, rbr, i};
    std::vector<Production> productions = {p1, p2, p3, p4};
    Grammar grammar(E, terminals, non_terminals, productions);
    ItemSetCollection item_set_collection(grammar);
    item_set_collection.GetLR1Table(std::cout);
}

int main(){
    test1();
}