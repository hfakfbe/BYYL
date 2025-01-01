// lr1_parser.h
#ifndef LR1_PARSER_H_
#define LR1_PARSER_H_

#include "lr1_table.h"

class LR1Parser {
protected:
    const LR1Table table_;

    void Print_(const std::vector<size_t> &state_stack, const std::vector<Symbol> &symbol_stack, const std::vector<Symbol> &token_stack) {
        std::cout << "State Stack: ";
        for(auto &state : state_stack) {
            std::cout << state << " ";
        }
        std::cout << std::endl;
        std::cout << "Symbol Stack: ";
        for(auto &symbol : symbol_stack) {
            std::cout << symbol.name << " ";
        }
        std::cout << std::endl;
        std::cout << "Token Stack: ";
        for(auto &token : token_stack) {
            std::cout << token.name << " ";
        }
        std::cout << std::endl << std::endl;
    }

public:
    LR1Parser(const LR1Table &table) : table_(table) {}

    void Parse(const std::vector<Symbol> &tokens) {
        // init stacks
        std::vector<size_t> state_stack;
        std::vector<Symbol> symbol_stack;
        std::vector<Symbol> token_stack;
        state_stack.push_back(0);
        symbol_stack.push_back(table_.GetEndSymbol());
        token_stack.push_back(table_.GetEndSymbol());
        for(int i = tokens.size() - 1; i >= 0; i --) {
            token_stack.push_back(tokens[i]);
        }
        // parse
        while(true) {
            // Print
            Print_(state_stack, symbol_stack, token_stack);
            // Get action
            size_t state = state_stack.back();
            Symbol token = token_stack.back();
            auto it = table_.GetAction().find({state, token});
            if(it == table_.GetAction().end()) {
                std::cerr << "Error: no action for state " << state << " and token " << token.name << std::endl;
                break;
            }
            auto action = it->second;
            // Do action
            if(action.first == 's') {
                state_stack.push_back(action.second);
                symbol_stack.push_back(token);
                token_stack.pop_back();
            }else if(action.first == 'r') {
                auto production = table_.GetGrammar().GetProductions()[action.second];
                for(int i = 0; i < production.right.size(); i ++) {
                    state_stack.pop_back();
                    symbol_stack.pop_back();
                }
                state = state_stack.back();
                symbol_stack.push_back(production.left);
                auto it = table_.GetGoto().find({state, production.left});
                if(it == table_.GetGoto().end()) {
                    std::cerr << "Error: no goto for state " << state << " and symbol " << production.left.name << std::endl;
                    break;
                }
                state_stack.push_back(it->second);
            }else if(action.first == 'a') {
                std::cout << "Accept" << std::endl;
                break;
            }else{
                std::cerr << "Error: unknown action " << action.first << std::endl;
                break;
            }
        }
    }

};

#endif  // LR1_PARSER_H_