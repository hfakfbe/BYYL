#include "grammar.h"

std::set<std::shared_ptr<Symbol>> Grammar::GetNonTerminals() const {
    std::set<std::shared_ptr<Symbol>> non_terminals;
    for(auto &production : productions_){
        non_terminals.insert(production->GetHead());
        for(auto &symbol : production->GetBody()){
            if(!symbol->is_terminal){
                non_terminals.insert(symbol);
            }
        }
    }
    return non_terminals;
}

std::set<std::shared_ptr<Symbol>> Grammar::GetTerminals() const {
    std::set<std::shared_ptr<Symbol>> terminals;
    for(auto &production : productions_){
        for(auto &symbol : production->GetBody()){
            if(symbol->is_terminal) {
                terminals.insert(symbol);
            }
        }
    }
    return terminals;
}

void Grammar::ComputeFollowSets() {
    
}