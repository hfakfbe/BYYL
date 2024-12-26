// goto_table.h
#ifndef GOTO_TABLE_H_
#define GOTO_TABLE_H_

#include <unordered_map>
#include <string>

class GotoTable {
private:
    std::unordered_map<int, std::unordered_map<std::string, int>> table_;

public:
    void AddEntry(int state, const std::string& nonterminal, int next_state) { table_[state][nonterminal] = next_state; }
    int GetGoto(int state, const std::string& nonterminal) const { return table_[state][nonterminal]; }
    bool HasEntry(int state, const std::string& symbol) const { return table_[state].find(symbol) != table_[state].end(); }
};

#endif  // GOTO_TABLE_H_