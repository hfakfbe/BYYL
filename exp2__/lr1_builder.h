// lr1_builder.h
#ifndef LR1_BUILDER_H_
#define LR1_BUILDER_H_

#include <memory>
#include <vector>
#include <unordered_map>
#include "grammar.h"
#include "item_set.h"
#include "action_table.h"
#include "goto_table.h"

class Lr1Builder {
private:
    std::unique_ptr<Grammar> grammar_;
    std::unordered_map<int, ItemSet> item_sets_;  // 状态编号 -> 项目集的映射
    std::unique_ptr<ActionTable> action_table_;
    std::unique_ptr<GotoTable> goto_table_;
    int current_state_id_;

public:
    Lr1Builder(std::unique_ptr<Grammar> grammar)
        : grammar_(std::move(grammar)), current_state_id_(0) {}

    // 构建LR(1)项目集和分析表
    void Build();

    // 获取构建好的动作表
    const ActionTable* GetActionTable() const { return action_table_.get(); }

    // 获取构建好的转移动作表
    const GotoTable* GetGotoTable() const { return goto_table_.get(); }

private:
    // 初始化项目集，从起始符号开始
    void InitializeItemSets();

    // 处理一个项目集，生成新的项目集并更新分析表
    void ProcessItemSet(const ItemSet& item_set, int state_id);

    // 根据项目集和文法更新动作表
    void UpdateActionTable(const ItemSet& item_set, int state_id);

    // 根据项目集和文法更新转移动作表
    void UpdateGotoTable(const ItemSet& item_set, int state_id);

    // 分配新的状态编号
    int AllocateNewStateId();
};

#endif  // LR1_BUILDER_H_