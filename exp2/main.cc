// main.cc
#include <iostream>
#include <memory>
#include <vector>
#include "symbol.h"
#include "production.h"
#include "grammar.h"
#include "action_table.h"
#include "goto_table.h"
#include "parser_state.h"
#include "l_attribute_parser.h"

int main() {
    // 创建符号
    auto e = std::make_shared<Symbol>("E", false);
    auto t = std::make_shared<Symbol>("T", false);
    auto f = std::make_shared<Symbol>("F", false);
    auto plus = std::make_shared<Symbol>("+", true);
    auto star = std::make_shared<Symbol>("*", true);
    auto lparen = std::make_shared<Symbol>("(", true);
    auto rparen = std::make_shared<Symbol>(")", true);
    auto id = std::make_shared<Symbol>("id", true);
    auto dollar = std::make_shared<Symbol>("$", true);

    // 创建文法
    auto grammar = std::make_unique<Grammar>(e);
    grammar->AddProduction(std::make_shared<Production>(e, {e, plus, t}));
    grammar->AddProduction(std::make_shared<Production>(e, {t}));
    grammar->AddProduction(std::make_shared<Production>(t, {t, star, f}));
    grammar->AddProduction(std::make_shared<Production>(t, {f}));
    grammar->AddProduction(std::make_shared<Production>(f, {lparen, e, rparen}));
    grammar->AddProduction(std::make_shared<Production>(f, {id}));

    // 构建项目集和分析表（省略具体实现）
    // ...

    // 创建解析器
    auto action_table = std::make_unique<ActionTable>();
    auto goto_table = std::make_unique<GotoTable>();
    auto parser = std::make_unique<LAttributeParser>(std::move(grammar), std::move(action_table), std::move(goto_table));

    // 解析输入字符串
    std::vector<std::string> input = {"id", "+", "id", "$"};
    if (parser->Parse(input)) {
        std::cout << "Parsing successful." << std::endl;
    } else {
        std::cout << "Syntax error." << std::endl;
    }

    return 0;
}