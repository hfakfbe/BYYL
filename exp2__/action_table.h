// action_table.h
#ifndef ACTION_TABLE_H_
#define ACTION_TABLE_H_

#include <unordered_map>
#include <string>

class ActionTable {
private:
    std::unordered_map<int, std::unordered_map<std::string, std::string>> table_;

public:
    void AddEntry(int state, const std::string& symbol, const std::string& action) { table_[state][symbol] = action; }
    std::string GetAction(int state, const std::string& symbol) const { return table_[state][symbol]; }
    bool HasEntry(int state, const std::string& symbol) const { return table_[state].find(symbol) != table_[state].end(); }
};

#endif  // ACTION_TABLE_H_