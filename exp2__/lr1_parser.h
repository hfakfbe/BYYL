// lr1_parser.h
#ifndef LR1_PARSER_H_
#define LR1_PARSER_H_

#include <memory>
#include <vector>
#include <string>
#include "grammar.h"
#include "action_table.h"
#include "goto_table.h"

class Lr1Parser {
protected:
    std::unique_ptr<Grammar> grammar_;
    std::unique_ptr<ActionTable> action_table_;
    std::unique_ptr<GotoTable> goto_table_;

public:
    Lr1Parser(std::unique_ptr<Grammar> grammar, std::unique_ptr<ActionTable> action_table, std::unique_ptr<GotoTable> goto_table)
        : grammar_(std::move(grammar)), action_table_(std::move(action_table)), goto_table_(std::move(goto_table)) {}

    virtual bool Parse(const std::vector<std::string>& input_symbols);

private:
    virtual void Shift(std::vector<int>& stack, std::vector<std::string>& input, size_t& index);
    virtual void Reduce(std::vector<int>& stack, std::vector<std::string>& input, size_t& index, const std::string& production_index);
    virtual void Accept();
    virtual void Error(const std::string& message);
};

#endif  // LR1_PARSER_H_