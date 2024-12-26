// l_attribute_parser.h
#ifndef L_ATTRIBUTE_PARSER_H_
#define L_ATTRIBUTE_PARSER_H_

#include "lr1_parser.h"

class LAttributeParser : public Lr1Parser {
public:
    LAttributeParser(std::unique_ptr<Grammar> grammar, std::unique_ptr<ActionTable> action_table, std::unique_ptr<GotoTable> goto_table)
        : Lr1Parser(std::move(grammar), std::move(action_table), std::move(goto_table)) {}

    // 重写parse方法以支持L属性计算
    bool Parse(const std::vector<std::string>& input_symbols) override;

    // 添加其他方法用于L属性计算和中间代码生成
};

#endif  // L_ATTRIBUTE_PARSER_H_