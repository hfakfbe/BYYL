#pragma once

#include "lexical_analysis.h"

#include <bits/stdc++.h>

class SymbolCFG : public Symbol
{
    private:
        int type;

    public:
        enum { NONTERMINAL = 1, TERMINAL };

        SymbolCFG(const std::string &name = std::string(), int type = 0);
        int get_type() const;
        bool operator<(const SymbolCFG &rhs) const;
};

class Rule
{
    private:
        SymbolCFG lhs;
        std::vector<SymbolCFG> rhs;

    public:
        Rule(const SymbolCFG &lhs, const std::vector<SymbolCFG> &rhs);
        const SymbolCFG &get_lhs() const;
        const std::vector<SymbolCFG> &get_rhs() const;
        bool operator<(const Rule &rhs) const;
};

class CFG
{
    private:
        std::set<SymbolCFG> symbols;
        std::set<Rule> rules;

    public:
        CFG(const std::set<SymbolCFG> &symbols, const std::set<Rule> &rules);
        const std::set<SymbolCFG> &get_symbols() const;
        const std::set<Rule> &get_rules() const;
};

class RuleLR1 : public Rule
{
    private:
        int dot_pos;
        Symbol next;

    public:
        RuleLR1(const SymbolCFG &lhs, const std::vector<SymbolCFG> &rhs, int dot_pos);
        int get_dot_pos() const;
        const Symbol &get_next() const;
        bool operator<(const RuleLR1 &rhs) const;
};

class ItemLR1
{
    private:
        std::set<RuleLR1> rules;

    public:
        ItemLR1();
        void add_rule(const RuleLR1 &rule);
        const std::vector<RuleLR1> &get_rules() const;
        bool operator<(const ItemLR1 &rhs) const;
};

class ItemCollectionLR1
{
    private:
        std::set<ItemLR1> items;
        std::map<std::pair<ItemLR1, SymbolCFG>, ItemLR1> go;

    public:
        ItemCollectionLR1();
        ItemCollectionLR1(const CFG &cfg);
};