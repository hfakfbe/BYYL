// parser_state.h
#ifndef PARSER_STATE_H_
#define PARSER_STATE_H_

#include "action_table.h"
#include "goto_table.h"

class ParserState {
private:
    std::unique_ptr<ActionTable> action_table_;
    std::unique_ptr<GotoTable> goto_table_;
    int current_state_;

public:
    ParserState(std::unique_ptr<ActionTable> action_table, std::unique_ptr<GotoTable> goto_table, int start_state = 0)
        : action_table_(std::move(action_table)), goto_table_(std::move(goto_table)), current_state_(start_state) {}
    
    const ActionTable* GetActionTable() const { return action_table_.get(); }
    const GotoTable* GetGotoTable() const { return goto_table_.get(); }
    int GetCurrentState() const { return current_state_; }
    void SetCurrentState(int state) { current_state_ = state; }
};

#endif  // PARSER_STATE_H_