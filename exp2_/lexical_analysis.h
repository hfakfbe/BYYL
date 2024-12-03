#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <array>
#include <string_view>

class State
{
    private:
        int id, type, attr;

    public:
        State(int id = 0, int type = GENERAL, int attr = 0);
        enum { START, OK, FAIL, GENERAL };
        int get_id() const;
        int get_type() const;
        int get_attr() const;
        bool operator==(const State &other) const;
        bool operator<(const State &other) const;
};

class Symbol
{
    private:
        std::string value;
    
    public:
        Symbol(std::string value = std::string());
        Symbol(char);
        std::string get_value() const;
        bool operator==(const Symbol &other) const;
        bool operator<(const Symbol &other) const;
};

class DFA
{
    private:
        std::map<State, int> states;
        std::map<Symbol, int> symbols;
        std::vector<std::vector<State>> transition_function;
        State current_state;
        
    public:
        DFA(std::vector<State>, std::vector<Symbol>, std::vector<std::vector<State>>);
        int reset();
        int go_next_state(Symbol);
        State get_current_state();
};

class LexicalAnalyzer
{
    private:
        DFA dfa;
        std::vector<std::pair<std::string, int>> result;
        int error_code;

    public:
        enum { OK = 0, UNRECOGNIZED_SYMBOL = -100, UNRECOGNIZED_IDENTIFIER, IGNORE, UNKNOWN_ERROR };
        
        LexicalAnalyzer(DFA);
        int set_dfa(DFA);
        int analyze(std::string);
        int reset();
        std::vector<std::pair<std::string, int>> get_result();
        int get_error();
};