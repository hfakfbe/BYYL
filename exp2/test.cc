#include <iostream>
#include <fstream>
#include "item_set_collection.h"
#include "lr1_parser.h"

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
    item_set_collection.OutLR1Table(std::cout);
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
    item_set_collection.OutLR1Table(std::cout);
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
    item_set_collection.OutLR1Table(std::cout);
}

void test4(){
    Symbol R("R", Symbol::Type::NONTERMINAL);
    Symbol unio("|", Symbol::Type::TERMINAL);
    Symbol star("*", Symbol::Type::TERMINAL);
    Symbol lbr("(", Symbol::Type::TERMINAL);
    Symbol rbr(")", Symbol::Type::TERMINAL);
    Symbol a("a", Symbol::Type::TERMINAL);
    Production p1(R, {R, unio, R});
    Production p2(R, {R, R});
    Production p3(R, {R, star});
    Production p4(R, {lbr, R, rbr});
    Production p5(R, {a});
    std::set<Symbol> non_terminals = {R};
    std::set<Symbol> terminals = {unio, star, lbr, rbr, a};
    std::vector<Production> productions = {p1, p2, p3, p4, p5};
    Grammar grammar(R, terminals, non_terminals, productions);
    ItemSetCollection item_set_collection(grammar);
    item_set_collection.OutLR1Table(std::cout);
}

void test5(){
    Symbol E("E", Symbol::Type::NONTERMINAL);
    Symbol ml("*", Symbol::Type::TERMINAL);
    Symbol pw("^", Symbol::Type::TERMINAL);
    Symbol lbr("(", Symbol::Type::TERMINAL);
    Symbol rbr(")", Symbol::Type::TERMINAL);
    Symbol i("i", Symbol::Type::TERMINAL);
    Production p1(E, {E, ml, E});
    Production p2(E, {E, pw, E});
    Production p3(E, {lbr, E, rbr});
    Production p4(E, {i});
    std::set<Symbol> non_terminals = {E};
    std::set<Symbol> terminals = {ml, pw, lbr, rbr, i};
    std::vector<Production> productions = {p1, p2, p3, p4};
    Grammar grammar(E, terminals, non_terminals, productions);
    ItemSetCollection item_set_collection(grammar);
    item_set_collection.OutLR1Table(std::cout);
}

void lr1_parser_test(){
    Symbol S("S", Symbol::Type::NONTERMINAL);
    Symbol A("A", Symbol::Type::NONTERMINAL);
    Symbol a("a", Symbol::Type::TERMINAL);
    Symbol b("b", Symbol::Type::TERMINAL);
    Symbol c("c", Symbol::Type::TERMINAL);
    Production p1(S, {a, A});
    Production p2(A, {b, A});
    Production p3(A, {c});
    std::set<Symbol> non_terminals = {S, A};
    std::set<Symbol> terminals = {a, b, c};
    std::vector<Production> productions = {p1, p2, p3};
    Grammar grammar(S, terminals, non_terminals, productions);
    ItemSetCollection item_set_collection(grammar);
    grammar = item_set_collection.GetGrammar(); // augment grammar

    std::fstream file("test.csv", std::ios::out);
    if(!file.is_open()){
        std::cerr << "Not opened" << std::endl;
    }
    item_set_collection.OutLR1Table(file);
    file.close();
    file.open("test.csv", std::ios::in);
    LR1Table table(file, grammar);
    LR1Parser parser(table);
    parser.Parse({a, b, b, c});
}

int main(){
    lr1_parser_test();
}