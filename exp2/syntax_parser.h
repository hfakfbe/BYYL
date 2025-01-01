// syntax_lr1_parser.h
#ifndef SYNTAX_PARSER_H_
#define SYNTAX_PARSER_H_

// The classes in this file all depends on the Grammar

#include <functional>
#include "lr1_parser.h"

class SyntaxSymbol : public Symbol {
public:
    int width;
    size_t quad, place, nextlist, truelist, falselist; // save indices
    std::string name, type, op;

    SyntaxSymbol(const std::string &name, Type type) : Symbol(name, type) {}

    SyntaxSymbol(const Symbol &symbol) : Symbol(symbol) {}
};

class SymbolTableEntry {
public:
    std::string name, type, value;
    int offset;
};

// Intermediate code
class ICode {
public:
    std::vector<SymbolTableEntry> symbol_table;
    std::vector<std::array<std::string, 4>> quad;
};

class SyntaxAttrRunner {
private:
    std::vector<SyntaxSymbol> symbol_stack_;

    ICode icode_;
    int nxq_, offset_, temp_count_;

    void BackPatch_(size_t qid, int nxq){
        // 回填函数，将 quad[qid] 的第四个元素（跳转目标）设置为 nxq
        for (size_t i = 0; i < icode_.quad.size(); ++i) {
            if (std::to_string(qid) == icode_.quad[i][3]) {
                icode_.quad[i][3] = std::to_string(nxq);
            }
        }
    }
    void Enter_(const std::string& name, const std::string& type, int offset){
        // 在符号表中创建新条目
        SymbolTableEntry entry;
        entry.name = name;
        entry.type = type;
        entry.offset = offset;
        icode_.symbol_table.push_back(entry);
    }
    void Gen_(std::string op, std::string arg1, std::string arg2, std::string result){
        // 生成四元式
        icode_.quad.push_back({op, arg1, arg2, result});
        nxq_++;
    }
    size_t Merge_(const size_t& head1, const size_t& head2) {
        if(head1 == -1) {
            return head2;
        }
        if(head2 == -1) {
            return head1;
        }
        size_t head = head2;
        while(icode_.quad[head][3] != "null") {
            head = std::stoi(icode_.quad[head][3]);
        }
        icode_.quad[head][3] = std::to_string(head1);
        return head2;
    }

    size_t NewTemp_(std::string type){
        std::string temp_name = "T" + std::to_string(temp_count_ ++) + "_" + type[0];
        SymbolTableEntry entry;
        entry.name = temp_name;
        entry.type = "temp"; // 可以根据需要设置类型
        entry.offset = -1;    // 临时变量通常不分配实际的内存地址
        icode_.symbol_table.push_back(entry);
        return icode_.symbol_table.size() - 1; // 返回临时变量在符号表中的索引
    }

    const SymbolTableEntry &Lookup_(const std::string& name){
        // 查找符号表
        for (const auto& entry : icode_.symbol_table) {
            if (entry.name == name) {
                return entry;
            }
        }
        // 如果找不到，可以返回一个特殊的条目或者抛出异常
        SyntaxError_();
        return icode_.symbol_table[0];
    }

    size_t Mklist_(int quad_index){
        if(quad_index == -1) {
            return -1;
        }
        icode_.quad[quad_index][3] = "null";
        return quad_index;
    }

    void SyntaxError_(){
        std::cout << "Syntax Error";
        exit(1);
    }

    std::map<Production, std::function<void()>> actions_;

public:
    SyntaxAttrRunner(const std::vector<Production> &p) {
        // PROG -> SUBPROG {}
        actions_[p[0]] = [&](){

        };
        // SUBPROG -> M VARIABLES STATEMENT {backpatch(STATEMENT.nextlist,nxq);gen(End,-,-,-);}
        actions_[p[1]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 2;
            BackPatch_(symbol_stack_[top].nextlist, nxq_);
            Gen_("End", "-", "-", "-");
        };
        // M -> ^ {OFFSET=0}
        actions_[p[2]] = [&](){
            int top = symbol_stack_.size() - 2;
            int ntop = top + 1;
            offset_ = 0;
        };
        // N -> ^ {N.quad=nxq}
        actions_[p[3]] = [&](){
            int top = symbol_stack_.size() - 2;
            int ntop = top + 1;
            symbol_stack_[ntop].quad = nxq_;
        };
        // VARIABLES -> VARIABLES VARIABLE ; {}
        actions_[p[4]] = [&](){

        };
        // VARIABLES -> VARIABLE ; {}
        actions_[p[5]] = [&](){

        };
        // T -> int {T.type=int;T.width=4;}
        actions_[p[6]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].type = "int";
            symbol_stack_[ntop].width = 4;
        };
        // T -> double {T.type=double;T.width=8;}
        actions_[p[7]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].type = "double";
            symbol_stack_[ntop].width = 8;
        };
        // ID -> id {ID.name=id}
        actions_[p[8]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].name = symbol_stack_[top].name; // ID.name = id.name
        };
        // VARIABLE -> T ID {enter(ID.name,type=T.type,offset=OFFSET);OFFSET+=T.width;VARIABLE.type=T.type;VARIABLE.width=T.width}
        actions_[p[9]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 1;
            Enter_(symbol_stack_[top].name, symbol_stack_[top - 1].type, offset_);
            offset_ += symbol_stack_[top - 1].width;
            symbol_stack_[ntop].type = symbol_stack_[top - 1].type;
            symbol_stack_[ntop].width = symbol_stack_[top - 1].width;
        };
        // VARIABLE -> VARIABLE_1 , ID {enter(ID.name,type=VARIABLE_1.type,offset=OFFSET);OFFSET+=VARIABLE_1.width;VARIABLE.type=VARIABLE_1.type;VARIABLE.width=VARIABLE_1.width;}
        actions_[p[10]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 2;
            Enter_(symbol_stack_[top].name, symbol_stack_[top - 2].type, offset_);
            offset_ += symbol_stack_[top - 2].width;
            symbol_stack_[ntop].type = symbol_stack_[top - 2].type;
            symbol_stack_[ntop].width = symbol_stack_[top - 2].width;
        };
        // STATEMENT -> ASSIGN {STATEMENT.nextlist=mklist()}
        actions_[p[11]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].nextlist = Mklist_(-1);
        };
        // STATEMENT -> SCANF {STATEMENT.nextlist=mklist()}
        actions_[p[12]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].nextlist = Mklist_(-1);
        };
        // STATEMENT -> PRINTF {STATEMENT.nextlist=mklist()}
        actions_[p[13]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].nextlist = Mklist_(-1);
        };
        // STATEMENT -> ^ {STATEMENT.nextlist=mklist()}
        actions_[p[14]] = [&](){
            int top = symbol_stack_.size() - 2; // 注意这里是 -2，因为右部为空
            int ntop = top + 1;
            symbol_stack_[ntop].nextlist = Mklist_(-1);
        };
        // STATEMENT -> { L ; } {STATEMENT.nextlist=L.nextlist}
        actions_[p[15]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 2;
            symbol_stack_[ntop].nextlist = symbol_stack_[top - 1].nextlist;
        };
        // STATEMENT -> while N_1 B do N_2 STATEMENT_1 {backpatch(STATEMENT_1.nextlist,N_1.quad);backpatch(B.truelist,N_2.quad);STATEMENT.nextlist=B.falselist;gen(j,-,-,N_1.quad)}
        actions_[p[16]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 5;
            BackPatch_(symbol_stack_[top].nextlist, symbol_stack_[top - 4].quad);
            BackPatch_(symbol_stack_[top - 3].truelist, symbol_stack_[top - 1].quad);
            symbol_stack_[ntop].nextlist = symbol_stack_[top - 3].falselist;
            Gen_("j", "-", "-", std::to_string(symbol_stack_[top - 4].quad));
        };
        // STATEMENT -> if B then N STATEMENT_1 {backpatch(B.truelist,N.quad),STATEMENT.nextlist=merge(B.falselist,STATEMENT_1.nextlist)}
        actions_[p[17]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 3;
            BackPatch_(symbol_stack_[top - 2].truelist, symbol_stack_[top - 1].quad);
            symbol_stack_[ntop].nextlist = Merge_(symbol_stack_[top - 2].falselist, symbol_stack_[top].nextlist);
        };
        // ASSIGN -> ID = EXPR {p=lookup(ID.name);gen(=,EXPR.place,-,p) }
        actions_[p[18]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 2;
            const SymbolTableEntry& entry = Lookup_(symbol_stack_[top - 2].name);
            int p = entry.offset;
            Gen_("=", icode_.symbol_table[symbol_stack_[top].place].name, "-", std::to_string(p));
        };
        // L -> L_1 ; N STATEMENT {backpatch(L1.nextlist,N.quad),L.nextlist=STATEMENT.nextlist}
        actions_[p[19]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 3;
            BackPatch_(symbol_stack_[top - 3].nextlist, symbol_stack_[top - 1].quad);
            symbol_stack_[ntop].nextlist = symbol_stack_[top].nextlist;
        };
        // L -> STATEMENT {L.nextlist=STATEMENT.nextlist}
        actions_[p[20]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].nextlist = symbol_stack_[top].nextlist;
        };
        // EXPR -> EXPR_1 || ORITEM {EXPR.place=newtemp(int);EXPR.type=int;gen(||,EXPR_1.place,ORITEM.place,EXPR.place)}
        actions_[p[21]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 2;
            size_t temp_index = NewTemp_("int");
            symbol_stack_[ntop].place = temp_index;
            symbol_stack_[ntop].type = "int";
            Gen_("||", icode_.symbol_table[symbol_stack_[top-2].place].name, icode_.symbol_table[symbol_stack_[top].place].name, icode_.symbol_table[symbol_stack_[ntop].place].name);
        };
        // EXPR -> ORITEM {EXPR.place=ORITEM.place;EXPR.type=ORITEM.type}
        actions_[p[22]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].place = symbol_stack_[top].place;
            symbol_stack_[ntop].type = symbol_stack_[top].type;
        };
        // ORITEM -> ORITEM_1 && ANDITEM {ORITEM.place=newtemp(int);ORITEM.type=int;gen(&&,ORITEM_1.place,ANDITEM.place,ORITEM.place)}
        actions_[p[23]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 2;
            size_t temp_index = NewTemp_("int");
            symbol_stack_[ntop].place = temp_index;
            symbol_stack_[ntop].type = "int";
            Gen_("&&", icode_.symbol_table[symbol_stack_[top-2].place].name, icode_.symbol_table[symbol_stack_[top].place].name, icode_.symbol_table[symbol_stack_[ntop].place].name);
        };
        // ORITEM -> ANDITEM {ORITEM.place=ANDITEM.place;ORITEM.type=ANDITEM.type}
        actions_[p[24]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].place = symbol_stack_[top].place;
            symbol_stack_[ntop].type = symbol_stack_[top].type;
        };
        // ANDITEM -> NOITEM {ANDITEM.place=NOITEM.place;ANDITEM.type=NOITEM.type;}
        actions_[p[25]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].place = symbol_stack_[top].place;
            symbol_stack_[ntop].type = symbol_stack_[top].type;
        };
        // ANDITEM -> ! NOITEM {ANDITEM=newtemp(int);ANDITEM.type=int;gen(!,NOITEM.place,-,ANDITEM.place)}
        actions_[p[26]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 1;
            size_t temp_index = NewTemp_("int");
            symbol_stack_[ntop].place = temp_index;
            symbol_stack_[ntop].type = "int";
            Gen_("!", icode_.symbol_table[symbol_stack_[top].place].name, "-", icode_.symbol_table[symbol_stack_[ntop].place].name);
        };
        // NOITEM -> NOITEM_1 REL RELITEM {NOITEM.place=newtemp(int);NOITEM.type=int;gen(REL.op,NOITEM_1.place,RELITEM.place,NOITEM.place)}
        actions_[p[27]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 2;
            size_t temp_index = NewTemp_("int");
            symbol_stack_[ntop].place = temp_index;
            symbol_stack_[ntop].type = "int";
            Gen_(symbol_stack_[top - 1].op, icode_.symbol_table[symbol_stack_[top - 2].place].name, icode_.symbol_table[symbol_stack_[top].place].name, icode_.symbol_table[symbol_stack_[ntop].place].name);
        };
        // NOITEM -> RELITEM {NOITEM.place=RELITEM.place;NOITEM.type=RELITEM.type}
        actions_[p[28]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].place = symbol_stack_[top].place;
            symbol_stack_[ntop].type = symbol_stack_[top].type;
        };
        // RELITEM -> RELITEM_1 PLUS_MINUS ITEM {RELITEM.place=newtemp(RELITEM_1.type);RELITEM.type=RELITEM_1.type;gen(PLUS_MINUS.op,RELITEM_1.place,ITEM.place,RELITEM.place)}
        actions_[p[29]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 2;
            size_t temp_index = NewTemp_(symbol_stack_[top - 2].type);
            symbol_stack_[ntop].place = temp_index;
            symbol_stack_[ntop].type = symbol_stack_[top - 2].type;
            Gen_(symbol_stack_[top - 1].op, icode_.symbol_table[symbol_stack_[top - 2].place].name, icode_.symbol_table[symbol_stack_[top].place].name, icode_.symbol_table[symbol_stack_[ntop].place].name);
        };
        // RELITEM -> ITEM {RELITEM.place=ITEM.place;RELITEM.type=ITEM.type}
        actions_[p[30]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].place = symbol_stack_[top].place;
            symbol_stack_[ntop].type = symbol_stack_[top].type;
        };
        // ITEM -> FACTOR {ITEM.place=FACTOR.place;ITEM.type=FACTOR.type}
        actions_[p[31]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].place = symbol_stack_[top].place;
            symbol_stack_[ntop].type = symbol_stack_[top].type;
        };
        // ITEM -> ITEM MUL_DIV FACTOR {ITEM.place=newtemp(FACTOR.type);ITEM.type=FACTOR.type;gen(MUL_DIV.op,ITEM_1.place,FACTOR.place,ITEM.place)}
        actions_[p[32]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 2;
            size_t temp_index = NewTemp_(symbol_stack_[top].type);
            symbol_stack_[ntop].place = temp_index;
            symbol_stack_[ntop].type = symbol_stack_[top].type;
            Gen_(symbol_stack_[top - 1].op, icode_.symbol_table[symbol_stack_[top - 2].place].name, icode_.symbol_table[symbol_stack_[top].place].name, icode_.symbol_table[symbol_stack_[ntop].place].name);
        };
        // FACTOR -> ID {FACTOR.place=lookup(ID.name);FACTOR.type=lookup_type(ID.name)}
        actions_[p[33]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            const SymbolTableEntry& entry = Lookup_(symbol_stack_[top].name);
            symbol_stack_[ntop].place = icode_.symbol_table.size() - 1; // place 应该是 entry 在符号表中的索引
            symbol_stack_[ntop].type = entry.type;
        };
        // FACTOR -> UINT {FACTOR.place=newtemp(int);FACTOR.type=int;gen(=,UINT,-,FACTOR.place)}
        actions_[p[34]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            size_t temp_index = NewTemp_("int");
            symbol_stack_[ntop].place = temp_index;
            symbol_stack_[ntop].type = "int";
            Gen_("=", symbol_stack_[top].name, "-", icode_.symbol_table[symbol_stack_[ntop].place].name);
        };
        // FACTOR -> UFLOAT {FACTOR.place=newtemp(double);FACTOR.type=double;gen(=,UFLOAT,-,FACTOR.place)}
        actions_[p[35]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            size_t temp_index = NewTemp_("double");
            symbol_stack_[ntop].place = temp_index;
            symbol_stack_[ntop].type = "double";
            Gen_("=", symbol_stack_[top].name, "-", icode_.symbol_table[symbol_stack_[ntop].place].name);
        };
        // FACTOR -> ( EXPR ) {FACTOR.place=EXPR.place;FACTOR.type=EXPR.type}
        actions_[p[36]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 2;
            symbol_stack_[ntop].place = symbol_stack_[top - 1].place;
            symbol_stack_[ntop].type = symbol_stack_[top - 1].type;
        };
        // FACTOR -> PLUS_MINUS FACTOR_1 {FACTOR.place=newtemp(FACTOR_1.type);FACTOR.type=FACTOR_1.type;gen(PLUS_MINUS.op,0,FACTOR_1.place,FACTOR.place)}
        actions_[p[37]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 1;
            size_t temp_index = NewTemp_(symbol_stack_[top].type);
            symbol_stack_[ntop].place = temp_index;
            symbol_stack_[ntop].type = symbol_stack_[top].type;
            Gen_(symbol_stack_[top - 1].op, "0", icode_.symbol_table[symbol_stack_[top].place].name, icode_.symbol_table[symbol_stack_[ntop].place].name);
        };
        // B -> B_1 || N BORTERM {backpatch(B_1.falselist,N.quad);B.truelist=merge(B_1.truelist,BORTERM.truelist);B.falselist=BORTERM.falselist}
        actions_[p[38]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 3;
            BackPatch_(symbol_stack_[top - 3].falselist, symbol_stack_[top - 1].quad);
            symbol_stack_[ntop].truelist = Merge_(symbol_stack_[top - 3].truelist, symbol_stack_[top].truelist);
            symbol_stack_[ntop].falselist = symbol_stack_[top].falselist;
        };
        // B -> BORTERM {B.truelist=BORTERM.truelist;B.falselist=BORTERM.falselist}
        actions_[p[39]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].truelist = symbol_stack_[top].truelist;
            symbol_stack_[ntop].falselist = symbol_stack_[top].falselist;
        };
        // BORTERM -> BORTERM_1 && N BANDTERM {backpatch(BORTERM_1.truelist,N.quad);BORTERM.falselist=merge(BORTERM_1.falselist,BANDTERM.falselist;BORTERM.truelist=BANDTERM.truelist)}
        actions_[p[40]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 3;
            BackPatch_(symbol_stack_[top - 3].truelist, symbol_stack_[top - 1].quad);
            symbol_stack_[ntop].falselist = Merge_(symbol_stack_[top - 3].falselist, symbol_stack_[top].falselist);
            symbol_stack_[ntop].truelist = symbol_stack_[top].truelist;
        };
        // BORTERM -> BANDTERM {BORTERM.truelist=BANDTERM.truelist;BORTERM.falselist=BANDTERM.falselist}
        actions_[p[41]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].truelist = symbol_stack_[top].truelist;
            symbol_stack_[ntop].falselist = symbol_stack_[top].falselist;
        };
        // BANDTERM -> ( B ) {BANDTERM.truelist=B.truelist;BANDTERM.falselist=B.falselist}
        actions_[p[42]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 2;
            symbol_stack_[ntop].truelist = symbol_stack_[top - 1].truelist;
            symbol_stack_[ntop].falselist = symbol_stack_[top - 1].falselist;
        };
        // BANDTERM -> ! BANDTERM_1 {BANDTERM.truelist=BANDTERM_1.falselist;BANDTERM.falselist=BANDTERM_1.truelist}
        actions_[p[43]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 1;
            symbol_stack_[ntop].truelist = symbol_stack_[top].falselist;
            symbol_stack_[ntop].falselist = symbol_stack_[top].truelist;
        };
        // BANDTERM -> BFACTOR_1 REL BFACTOR_2 {BANDTERM.truelist=mklist(nxq);BANDTERM.falselist=mklist(nxq+1);gen(j+REL.op,BFACTOR_1.place,BFACTOR_2.place,0);gen(j,-,-,0);}
        actions_[p[44]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 2;
            Gen_("j" + symbol_stack_[top - 1].op, icode_.symbol_table[symbol_stack_[top-2].place].name, icode_.symbol_table[symbol_stack_[top].place].name, "0");
            Gen_("j", "-", "-", "0");
            symbol_stack_[ntop].truelist = Mklist_(nxq_ - 2);
            symbol_stack_[ntop].falselist = Mklist_(nxq_ - 1);
        };
        // BANDTERM -> BFACTOR {BANDTERM.truelist=mklist(nxq);BANDTERM.falselist=mklist(nxq+1);gen(jnz,BFACTOR.place,-,0);gen(j,-,-,0)}
        actions_[p[45]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            Gen_("jnz", icode_.symbol_table[symbol_stack_[top].place].name, "-", "0");
            Gen_("j", "-", "-", "0");
            symbol_stack_[ntop].truelist = Mklist_(nxq_ - 2);
            symbol_stack_[ntop].falselist = Mklist_(nxq_ - 1);
        };
        // BFACTOR -> UINT {BFACTOR.place=newtemp(int);BFACTOR.type=int;gen(=,UINT,-,FACTOR.place)}
        actions_[p[46]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            size_t temp_index = NewTemp_("int");
            symbol_stack_[ntop].place = temp_index;
            symbol_stack_[ntop].type = "int";
            Gen_("=", symbol_stack_[top].name, "-", icode_.symbol_table[symbol_stack_[ntop].place].name);
        };
        // BFACTOR -> UFLOAT {BFACTOR.place=newtemp(double);BFACTOR.type=double;gen(=,UFLOAT,-,BFACTOR.place)}
        actions_[p[47]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            size_t temp_index = NewTemp_("double");
            symbol_stack_[ntop].place = temp_index;
            symbol_stack_[ntop].type = "double";
            Gen_("=", symbol_stack_[top].name, "-", icode_.symbol_table[symbol_stack_[ntop].place].name);
        };
        // BFACTOR -> ID {BFACTOR.place=lookup(ID.name);BFACTOR.type=lookup_type(ID.name)}
        actions_[p[48]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            const SymbolTableEntry& entry = Lookup_(symbol_stack_[top].name);
            symbol_stack_[ntop].place = entry.offset; // 使用 offset
            symbol_stack_[ntop].type = entry.type;
        };
        // PLUS_MINUS -> + {PLUS_MINUS.op='+'}
        actions_[p[49]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].op = "+";
        };
        // PLUS_MINUS -> - { PLUS_MINUS.op='-'}
        actions_[p[50]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].op = "-";
        };
        // MUL_DIV -> * {MUL_DIV.op='*'}
        actions_[p[51]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].op = "*";
        };
        // MUL_DIV -> / { MUL_DIV.op='/'}
        actions_[p[52]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].op = "/";
        };
        // REL -> == {REL.op='=='}
        actions_[p[53]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].op = "==";
        };
        // REL -> != { REL.op='!='}}
        actions_[p[54]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].op = "!=";
        };
        // REL -> < { REL.op='<'}}
        actions_[p[55]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].op = "<";
        };
        // REL -> <= { REL.op='<='}
        actions_[p[56]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].op = "<=";
        };
        // REL -> > { REL.op='>'}}
        actions_[p[57]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].op = ">";
        };
        // REL -> >= { REL.op='>='}
        actions_[p[58]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top;
            symbol_stack_[ntop].op = ">=";
        };
        // SCANF -> SCANF_BEGIN ) {}
        actions_[p[59]] = [&](){

        };
        // SCANF_BEGIN -> SCANF_BEGIN , ID {p=lookup(ID.name);gen(R,-,-,p)}
        actions_[p[60]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 2;
            int p = Lookup_(symbol_stack_[top].name).offset;
            Gen_("R", "-", "-", std::to_string(p));
        };
        // SCANF_BEGIN -> scanf ( ID {p=lookup(ID.name);gen(R,-,-,p)}
        actions_[p[61]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 2;
            int p = Lookup_(symbol_stack_[top].name).offset;
            Gen_("R", "-", "-", std::to_string(p));
        };
        // PRINTF -> PRINTF_BEGIN ) {}
        actions_[p[62]] = [&](){

        };
        // PRINTF_BEGIN -> printf ( ID {p=lookup(ID.name);gen(W,-,-,p)}
        actions_[p[63]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 2;
            int p = Lookup_(symbol_stack_[top].name).offset;
            Gen_("W", "-", "-", std::to_string(p));
        };
        // PRINTF_BEGIN -> PRINTF_BEGIN , ID {p=lookup(ID.name);gen(W,-,-,p)}
        actions_[p[64]] = [&](){
            int top = symbol_stack_.size() - 1;
            int ntop = top - 2;
            int p = Lookup_(symbol_stack_[top].name).offset;
            Gen_("W", "-", "-", std::to_string(p));
        };

        Init();
    }

    void ClearStack() {
        symbol_stack_.clear();
    }

    void PushSymbol(const Symbol &symbol) {
        symbol_stack_.push_back(symbol);
    }

    const ICode &GetICode() {
        return icode_;
    }

    void Init() {
        nxq_ = 0;
        temp_count_ = 0;
        ClearStack();
        icode_.quad.clear();
        icode_.symbol_table.clear();
    }

    void RunAction(const Production &production){
        // the function should know if right is empty, push
        if(production.right.size() == 0){
            symbol_stack_.push_back(production.left);
        }
        actions_[production]();
        // reserve the space for the new symbol
        for(int i = 1; i < production.right.size(); i ++) {
            symbol_stack_.pop_back();
        }
        symbol_stack_.back().Symbol::name = production.left.name;
        symbol_stack_.back().Symbol::type = production.left.type;
    }

    void Print() {
        std::cout << "Symbol Stack: ";
        for(const auto &symbol : symbol_stack_) {
            std::cout << symbol.Symbol::name << " ";
        }
        std::cout << std::endl;
    }
};

class SyntaxParser : public LR1Parser {
private:
    SyntaxAttrRunner attr_runner_;

    void Print_(const std::vector<size_t> &state_stack, const std::vector<Symbol> &token_stack) {
        std::cout << "State Stack: ";
        for(const auto &state : state_stack) {
            std::cout << state << " ";
        }
        std::cout << std::endl;
        attr_runner_.Print();
        std::cout << "Token Stack: ";
        for(const auto &token : token_stack) {
            std::cout << token.name << " ";
        }
        std::cout << std::endl << std::endl;
    }
public:
    SyntaxParser(const LR1Table &table, const std::vector<Production> &p)
         : LR1Parser(table), attr_runner_(p) {}

    ICode Parse(const std::vector<Symbol> &tokens) {
        // init stacks
        attr_runner_.Init();
        std::vector<size_t> state_stack;
        std::vector<Symbol> token_stack;
        state_stack.push_back(0);
        attr_runner_.PushSymbol(table_.GetEndSymbol());
        token_stack.push_back(table_.GetEndSymbol());
        for(int i = tokens.size() - 1; i >= 0; i --) {
            token_stack.push_back(tokens[i]);
        }
        // parse
        while(true) {
            // Print_(state_stack, token_stack);
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
                attr_runner_.PushSymbol(token);
                token_stack.pop_back();
            }else if(action.first == 'r') {
                auto production = table_.GetGrammar().GetProductions()[action.second];
                // Run, pop and push symbol stack
                attr_runner_.RunAction(production);
                // Pop and push state stack
                for(int i = 0; i < production.right.size(); i ++) {
                    state_stack.pop_back();
                }
                state = state_stack.back();
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
        return attr_runner_.GetICode();
    }
};

#endif  // SYNTAX_PARSER_H_