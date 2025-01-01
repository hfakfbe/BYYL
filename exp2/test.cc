#include <iostream>
#include <fstream>
#include "item_set_collection.h"
#include "syntax_parser.h"

void test1(){
    Symbol S("S", Symbol::Type::NONTERMINAL);
    Symbol A("A", Symbol::Type::NONTERMINAL);
    Symbol B("B", Symbol::Type::NONTERMINAL);
    Symbol a("a", Symbol::Type::TERMINAL);
    Symbol b("b", Symbol::Type::TERMINAL);
    Production p1(S, {a, A, a, A, b});
    Production p2(S, {a, B, b});
    Production p3(A, {a});
    Production p4(B, {a});
    std::set<Symbol> non_terminals = {S, A, B};
    std::set<Symbol> terminals = {a, b};
    std::vector<Production> productions = {p1, p2, p3, p4};
    Grammar grammar(S, terminals, non_terminals, productions);
    ItemSetCollection item_set_collection(grammar);
    item_set_collection.OutLR1Table(std::cout);
}

void test2(){
    Symbol S("S", Symbol::Type::NONTERMINAL);
    Symbol Q("Q", Symbol::Type::NONTERMINAL);
    Symbol o("o", Symbol::Type::TERMINAL);
    Symbol p("p", Symbol::Type::TERMINAL);
    Production p1(S, {Q, Q});
    Production p2(Q, {o, Q});
    Production p3(Q, {p});
    std::set<Symbol> non_terminals = {S, Q};
    std::set<Symbol> terminals = {o, p};
    std::vector<Production> productions = {p1, p2, p3};
    Grammar grammar(S, terminals, non_terminals, productions);
    ItemSetCollection item_set_collection(grammar);
    item_set_collection.OutLR1Table(std::cout);
}

void test3(){
    Symbol E("E", Symbol::Type::NONTERMINAL);
    Symbol plus("+", Symbol::Type::TERMINAL);
    Symbol mul("*", Symbol::Type::TERMINAL);
    Symbol lbr("(", Symbol::Type::TERMINAL);
    Symbol rbr(")", Symbol::Type::TERMINAL);
    Symbol i("i", Symbol::Type::TERMINAL);
    Production p1(E, {E, plus, E});
    Production p2(E, {E, mul, E});
    Production p3(E, {lbr, E, rbr});
    Production p4(E, {i});
    std::set<Symbol> non_terminals = {E};
    std::set<Symbol> terminals = {plus, mul, lbr, rbr, i};
    std::vector<Production> productions = {p1, p2, p3, p4};
    Grammar grammar(E, terminals, non_terminals, productions);
    ItemSetCollection item_set_collection(grammar);
    item_set_collection.OutLR1Table(std::cout);
}

void test4(){
    Symbol R("R", Symbol::Type::NONTERMINAL);
    Symbol unio("|", Symbol::Type::TERMINAL);
    Symbol star("*", Symbol::Type::TERMINAL);
    Symbol lbr("(", Symbol::Type::TERMINAL);
    Symbol rbr(")", Symbol::Type::TERMINAL);
    Symbol a("a", Symbol::Type::TERMINAL);
    Production p1(R, {R, unio, R});
    Production p2(R, {R, R});
    Production p3(R, {R, star});
    Production p4(R, {lbr, R, rbr});
    Production p5(R, {a});
    std::set<Symbol> non_terminals = {R};
    std::set<Symbol> terminals = {unio, star, lbr, rbr, a};
    std::vector<Production> productions = {p1, p2, p3, p4, p5};
    Grammar grammar(R, terminals, non_terminals, productions);
    ItemSetCollection item_set_collection(grammar);
    item_set_collection.OutLR1Table(std::cout);
}

void test5(){
    Symbol E("E", Symbol::Type::NONTERMINAL);
    Symbol ml("*", Symbol::Type::TERMINAL);
    Symbol pw("^", Symbol::Type::TERMINAL);
    Symbol lbr("(", Symbol::Type::TERMINAL);
    Symbol rbr(")", Symbol::Type::TERMINAL);
    Symbol i("i", Symbol::Type::TERMINAL);
    Production p1(E, {E, ml, E});
    Production p2(E, {E, pw, E});
    Production p3(E, {lbr, E, rbr});
    Production p4(E, {i});
    std::set<Symbol> non_terminals = {E};
    std::set<Symbol> terminals = {ml, pw, lbr, rbr, i};
    std::vector<Production> productions = {p1, p2, p3, p4};
    Grammar grammar(E, terminals, non_terminals, productions);
    ItemSetCollection item_set_collection(grammar);
    item_set_collection.OutLR1Table(std::cout);
}

void lr1_parser_test(){
    Symbol S("S", Symbol::Type::NONTERMINAL);
    Symbol A("SCANF", Symbol::Type::NONTERMINAL);
    Symbol a("a", Symbol::Type::TERMINAL);
    Symbol b("b", Symbol::Type::TERMINAL);
    Symbol c("c", Symbol::Type::TERMINAL);
    Production p1(S, {a, A});
    Production p2(A, {b, A});
    Production p3(A, {c});
    std::set<Symbol> non_terminals = {S, A};
    std::set<Symbol> terminals = {a, b, c};
    std::vector<Production> productions = {p1, p2, p3};
    Grammar grammar(S, terminals, non_terminals, productions);
    ItemSetCollection item_set_collection(grammar);
    grammar = item_set_collection.GetGrammar(); // augment grammar

    std::fstream file("test.csv", std::ios::out);
    if(!file.is_open()){
        std::cerr << "Not opened" << std::endl;
    }
    item_set_collection.OutLR1Table(file);
    file.close();
    file.open("test.csv", std::ios::in);
    LR1Table table(file, grammar);
    LR1Parser parser(table);
    parser.Parse({a, b, b, c});
}

void real_test(){
    // Nonterminal
    Symbol PROG("PROG", Symbol::Type::NONTERMINAL);
    Symbol SUBPROG("SUBPROG", Symbol::Type::NONTERMINAL);
    Symbol M("M", Symbol::Type::NONTERMINAL);
    Symbol N("N", Symbol::Type::NONTERMINAL);
    Symbol VARIABLES("VARIABLES", Symbol::Type::NONTERMINAL);
    Symbol STATEMENT("STATEMENT", Symbol::Type::NONTERMINAL);
    Symbol VARIABLE("VARIABLE", Symbol::Type::NONTERMINAL);
    Symbol T("T", Symbol::Type::NONTERMINAL);
    Symbol ASSIGN("ASSIGN", Symbol::Type::NONTERMINAL);
    Symbol SCANF("SCANF", Symbol::Type::NONTERMINAL);
    Symbol PRINTF("PRINTF", Symbol::Type::NONTERMINAL);
    Symbol L("L", Symbol::Type::NONTERMINAL);
    Symbol B("B", Symbol::Type::NONTERMINAL);
    Symbol EXPR("EXPR", Symbol::Type::NONTERMINAL);
    Symbol ORITEM("ORITEM", Symbol::Type::NONTERMINAL);
    Symbol ANDITEM("ANDITEM", Symbol::Type::NONTERMINAL);
    Symbol RELITEM("RELITEM", Symbol::Type::NONTERMINAL);
    Symbol NOITEM("NOITEM", Symbol::Type::NONTERMINAL);
    Symbol ITEM("ITEM", Symbol::Type::NONTERMINAL);
    Symbol FACTOR("FACTOR", Symbol::Type::NONTERMINAL);
    Symbol BORTERM("BORTERM", Symbol::Type::NONTERMINAL);
    Symbol BANDTERM("BANDTERM", Symbol::Type::NONTERMINAL);
    Symbol PLUS_MINUS("PLUS_MINUS", Symbol::Type::NONTERMINAL);
    Symbol MUL_DIV("MUL_DIV", Symbol::Type::NONTERMINAL);
    Symbol REL("REL", Symbol::Type::NONTERMINAL);
    Symbol SCANF_BEGIN("SCANF_BEGIN", Symbol::Type::NONTERMINAL);
    Symbol PRINTF_BEGIN("PRINTF_BEGIN", Symbol::Type::NONTERMINAL);
    Symbol ID("ID", Symbol::Type::NONTERMINAL);
    Symbol BFACTOR("BFACTOR", Symbol::Type::NONTERMINAL);
    // Terminal
    Symbol t_int("int", Symbol::Type::TERMINAL);
    Symbol t_double("double", Symbol::Type::TERMINAL);
    Symbol t_scan("scanf", Symbol::Type::TERMINAL);
    Symbol t_print("printf", Symbol::Type::TERMINAL);
    Symbol t_if("if", Symbol::Type::TERMINAL);
    Symbol t_then("then", Symbol::Type::TERMINAL);
    Symbol t_while("while", Symbol::Type::TERMINAL);
    Symbol t_do("do", Symbol::Type::TERMINAL);
    Symbol t_comma(",", Symbol::Type::TERMINAL);
    Symbol t_semicolon(";", Symbol::Type::TERMINAL);
    Symbol t_plus("+", Symbol::Type::TERMINAL);
    Symbol t_minus("-", Symbol::Type::TERMINAL);
    Symbol t_mul("*", Symbol::Type::TERMINAL);
    Symbol t_div("/", Symbol::Type::TERMINAL);
    Symbol t_assign("=", Symbol::Type::TERMINAL);
    Symbol t_eq("==", Symbol::Type::TERMINAL);
    Symbol t_neq("!=", Symbol::Type::TERMINAL);
    Symbol t_lt("<", Symbol::Type::TERMINAL);
    Symbol t_leq("<=", Symbol::Type::TERMINAL);
    Symbol t_gt(">", Symbol::Type::TERMINAL);
    Symbol t_geq(">=", Symbol::Type::TERMINAL);
    Symbol t_lparen("(", Symbol::Type::TERMINAL);
    Symbol t_rparen(")", Symbol::Type::TERMINAL);
    Symbol t_lbrace("{", Symbol::Type::TERMINAL);
    Symbol t_rbrace("}", Symbol::Type::TERMINAL);
    Symbol t_not("!", Symbol::Type::TERMINAL);
    Symbol t_and("&&", Symbol::Type::TERMINAL);
    Symbol t_or("||", Symbol::Type::TERMINAL);
    Symbol t_id("id", Symbol::Type::TERMINAL);
    Symbol t_uint("UINT", Symbol::Type::TERMINAL);
    Symbol t_ufloat("UFLOAT", Symbol::Type::TERMINAL);
    // Production
    Production p1(PROG, {SUBPROG});
    Production p2(SUBPROG, {M, VARIABLES, STATEMENT});
    Production p3(M, {});
    Production p4(N, {});
    Production p5(VARIABLES, {VARIABLES, VARIABLE, t_semicolon});
    Production p6(VARIABLES, {VARIABLE, t_semicolon});
    Production p7(T, {t_int});
    Production p8(T, {t_double});
    Production p9(ID, {t_id});
    Production p10(VARIABLE, {T, ID});
    Production p11(VARIABLE, {VARIABLE, t_comma, ID});
    Production p12(STATEMENT, {ASSIGN});
    Production p13(STATEMENT, {SCANF});
    Production p14(STATEMENT, {PRINTF});
    Production p15(STATEMENT, {});
    Production p16(STATEMENT, {t_lbrace, L, t_semicolon, t_rbrace});
    Production p17(STATEMENT, {t_while, N, B, t_do, N, STATEMENT});
    Production p18(STATEMENT, {t_if, B, t_then, N, STATEMENT});
    Production p19(ASSIGN, {ID, t_assign, EXPR});
    Production p20(L, {L, t_semicolon, N, STATEMENT});
    Production p21(L, {STATEMENT});
    Production p22(EXPR, {EXPR, t_or, ORITEM});
    Production p23(EXPR, {ORITEM});
    Production p24(ORITEM, {ORITEM, t_and, ANDITEM});
    Production p25(ORITEM, {ANDITEM});
    Production p26(ANDITEM, {NOITEM});
    Production p27(ANDITEM, {t_not, NOITEM});
    Production p28(NOITEM, {NOITEM, REL, RELITEM});
    Production p29(NOITEM, {RELITEM});
    Production p30(RELITEM, {RELITEM, PLUS_MINUS, ITEM});
    Production p31(RELITEM, {ITEM});
    Production p32(ITEM, {FACTOR});
    Production p33(ITEM, {ITEM, MUL_DIV, FACTOR});
    Production p34(FACTOR, {ID});
    Production p35(FACTOR, {t_uint});
    Production p36(FACTOR, {t_ufloat});
    Production p37(FACTOR, {t_lparen, EXPR, t_rparen});
    Production p38(FACTOR, {PLUS_MINUS, FACTOR});
    Production p39(B, {B, t_or, N, BORTERM});
    Production p40(B, {BORTERM});
    Production p41(BORTERM, {BORTERM, t_and, N, BANDTERM});
    Production p42(BORTERM, {BANDTERM});
    Production p43(BANDTERM, {t_lparen, B, t_rparen});
    Production p44(BANDTERM, {t_not, BANDTERM});
    Production p45(BANDTERM, {BFACTOR, REL, BFACTOR});
    Production p46(BANDTERM, {BFACTOR});
    Production p47(BFACTOR, {t_uint});
    Production p48(BFACTOR, {t_ufloat});
    Production p49(BFACTOR, {ID});
    Production p50(PLUS_MINUS, {t_plus});
    Production p51(PLUS_MINUS, {t_minus});
    Production p52(MUL_DIV, {t_mul});
    Production p53(MUL_DIV, {t_div});
    Production p54(REL, {t_eq});
    Production p55(REL, {t_neq});
    Production p56(REL, {t_lt});
    Production p57(REL, {t_leq});
    Production p58(REL, {t_gt});
    Production p59(REL, {t_geq});
    Production p60(SCANF, {SCANF_BEGIN, t_rparen});
    Production p61(SCANF_BEGIN, {SCANF_BEGIN, t_comma, ID});
    Production p62(SCANF_BEGIN, {t_scan, t_lparen, ID});
    Production p63(PRINTF, {PRINTF_BEGIN, t_rparen});
    Production p64(PRINTF_BEGIN, {t_print, t_lparen, ID});
    Production p65(PRINTF_BEGIN, {PRINTF_BEGIN, t_comma, ID});
    std::set<Symbol> non_terminals = {PROG, SUBPROG, M, N, VARIABLES, STATEMENT, VARIABLE, T, ASSIGN, SCANF, PRINTF, L, B, EXPR, ORITEM, ANDITEM, RELITEM, NOITEM, ITEM, FACTOR, BORTERM, BANDTERM, PLUS_MINUS, MUL_DIV, REL, SCANF_BEGIN, PRINTF_BEGIN, ID, BFACTOR};
    std::set<Symbol> terminals = {t_int, t_double, t_scan, t_print, t_if, t_then, t_while, t_do, t_comma, t_semicolon, t_plus, t_minus, t_mul, t_div, t_assign, t_eq, t_neq, t_lt, t_leq, t_gt, t_geq, t_lparen, t_rparen, t_lbrace, t_rbrace, t_not, t_and, t_or, t_id, t_uint, t_ufloat};
    std::vector<Production> productions = {p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57, p58, p59, p60, p61, p62, p63, p64, p65};
    Grammar grammar(PROG, terminals, non_terminals, productions);
    ItemSetCollection item_set_collection(grammar);

    std::fstream file("test.csv", std::ios::out);
    item_set_collection.OutLR1Table(file);
    // item_set_collection.OutItems(std::cout);
}

int main(){
    real_test();
    // lr1_parser_test();
}