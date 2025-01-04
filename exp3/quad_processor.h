// quad_processor.h
#ifndef QUAD_PROCESSOR_H_
#define QUAD_PROCESSOR_H_

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <set>
#include <sstream>
#include <cassert>
#include <climits>

// 定义四元式结构
class Quadruple {
public:
    std::string op;
    std::string arg1;
    std::string arg2;
    std::string result;
    int use_arg1 = -1;
    int use_arg2 = -1;
    int use_result = -1;
    bool live_arg1 = false;
    bool live_arg2 = false;
    bool live_result = false;

    Quadruple(std::string op, std::string arg1, std::string arg2, std::string result)
         : op(op), arg1(arg1), arg2(arg2), result(result) {}
};

class Register {
public:
    std::string name;
};

// 定义符号表条目
class SymbolTableEntry {
public:
    std::string name;
    int type;
    std::string value;
    int offset = -1;    // 内存偏移量

    int use = -1;    // 下次引用点
    bool live = false;  // 活跃信息
    
    SymbolTableEntry(std::string name) : name(name) {}
};

// 定义基本块结构
class BasicBlock {
public:
    int start; // 基本块起始四元式编号
    int end;   // 基本块结束四元式编号
    bool have_label = false; // 是否有标号
};

class QuadProcessor {
public:
    // 输入：quadruples - 四元式序列, symbolTable - 符号表
    // 作用：构造函数，初始化四元式处理器
    QuadProcessor(std::vector<Quadruple> &quadruples, std::vector<SymbolTableEntry> &symbolTable)
         : quadruples_(quadruples), symbol_table_(symbolTable) {
        // 初始化偏移量
        auto entry = symbol_table_.back();
        offset_ = entry.offset + 4 + entry.type * 4;
        // 初始化寄存器
        registers_.push_back(Register{"R0"});
        registers_.push_back(Register{"R1"});
        registers_.push_back(Register{"R2"});
        // 初始化寄存器描述符和地址描述符
        for(auto &reg : registers_){
            r_val_[reg.name] = {};
        }
        for(auto &entry : symbol_table_){
            a_val_[entry.name] = {};
        }
        // 找临时变量
        for(auto &quad : quadruples_){
            for(auto str : {quad.arg1, quad.arg2, quad.result}){
                if(str[0] == 'T' && str[1] != 'B'){
                    auto it = std::find_if(symbol_table_.begin(), symbol_table_.end(), [&](SymbolTableEntry &entry){
                        return entry.name == str;
                    });
                    if(it == symbol_table_.end()){
                        InsertSym_(str);
                        a_val_[str] = {};
                    }
                }
            }
        }
    }

    // 输入：无
    // 输出：无
    // 作用：主处理函数，控制整个四元式处理流程，包括划分基本块，计算待用信息，生成目标代码等
    std::string ProcessQuadruples(){
        DivBlocks_();
        CalcLiveUse_();
        return GenTarget_();
    }

private:
    std::vector<Quadruple>& quadruples_;
    std::vector<BasicBlock> basic_blocks_;

    std::vector<SymbolTableEntry>& symbol_table_;
    int offset_;
    std::vector<Register> registers_;

    // 寄存器描述符: 寄存器 -> 变量
    std::map<std::string, std::set<std::string>> r_val_;
    // 地址描述符: 变量 -> 寄存器/内存地址
    std::map<std::string, std::set<std::string>> a_val_;

    std::stringstream target_;

    // 输入：无
    // 输出：basicBlocks - 划分好的基本块
    // 作用：将四元式序列划分为基本块
    void DivBlocks_(){
        int n = quadruples_.size();
        std::set<int> blockstarts, labelstarts;
        blockstarts.insert(0);
        for(int i = 0; i < n; i++){
            if(quadruples_[i].op[0] == 'j'){
                int target = std::stoi(quadruples_[i].result);
                blockstarts.insert(target);
                labelstarts.insert(target);
                if(i < n - 1){
                    blockstarts.insert(i + 1);
                }
            }else if(quadruples_[i].op == "R" || quadruples_[i].op == "W"){
                blockstarts.insert(i);
            }
        }
        int i = 0;
        while(i < n){
            if(blockstarts.find(i) != blockstarts.end()){
                if(i == n - 1){
                    basic_blocks_.push_back(BasicBlock{i, i});
                    break;
                }
                for(int j = i + 1; j < n; j++){
                    if(blockstarts.find(j) != blockstarts.end()){
                        basic_blocks_.push_back(BasicBlock{i, j - 1});
                        i = j;
                        break;
                    }else if(quadruples_[j].op[0] == 'j' || quadruples_[j].op == "End"){
                        basic_blocks_.push_back(BasicBlock{i, j});
                        i = j + 1;
                        break;
                    }
                }
            }else{
                i ++;
            }
        }
        for(auto &block : basic_blocks_){
            if(labelstarts.count(block.start)){
                block.have_label = true;
            }
        }
    }

    // 输入：无
    // 输出：更新后的四元式和符号表中的待用信息
    // 作用：计算每个基本块内四元式的待用信息
    void CalcLiveUse_(){
        for(int i = 0; i < basic_blocks_.size(); i++){
            CalcLiveUseB_(i);
        }
    }

    // 输入：blockId - 基本块编号
    // 输出：更新后的四元式和符号表中的待用信息
    // 作用：计算指定基本块内四元式的待用信息
    void CalcLiveUseB_(int blockid){
        auto block = basic_blocks_[blockid];
        // 活动变量为所有非临时变量
        for(int i = block.end; i >= block.start; i--){
            auto quad = quadruples_[i];
            if(quad.op == "End"){
                continue;
            }
            for(auto name : {quad.arg1, quad.arg2, quad.result}){
                auto it = std::find_if(symbol_table_.begin(), symbol_table_.end(), [&](SymbolTableEntry &entry){
                    return entry.name == name;
                });
                if(it != symbol_table_.end()){
                    it->use = -1;
                    it->live = it->name[1] == 'B';
                }
            }
        }
        // 计算待用信息
        for(int i = block.end; i >= block.start; i--){
            auto quad = quadruples_[i];
            if(quad.op == "End"){
                continue;
            }
            auto it = std::find_if(symbol_table_.begin(), symbol_table_.end(), [&](SymbolTableEntry &entry){
                return entry.name == quad.result;
            });
            if(it != symbol_table_.end()){
                quad.use_result = it->use;
                quad.live_result = it->live;
                it->use = -1;
                it->live = false;
            }
            for(auto name : {quad.arg1, quad.arg2}){
                auto it = std::find_if(symbol_table_.begin(), symbol_table_.end(), [&](SymbolTableEntry &entry){
                    return entry.name == name;
                });
                if(it != symbol_table_.end()){
                    quad.use_arg1 = it->use;
                    quad.live_arg1 = it->live;
                    it->use = i;
                    it->live = true;
                }
            }
        }
    }

    // 输入：无
    // 输出：目标代码
    // 作用：生成目标代码
    std::string GenTarget_(){
        target_.clear();
        for(int i = 0; i < basic_blocks_.size(); i ++){
            auto block = basic_blocks_[i];
            if(block.have_label){
                target_ << "?" << i << ":" << std::endl;
            }
            for(int j = block.start; j <= block.end; j ++){
                auto quad = quadruples_[j];
                if(CheckTheta_(quad.op)){
                    std::string rz = GetReg_(j);
                    std::string x = GetVal_(quad.arg1);
                    std::string y = GetVal_(quad.arg2);
                    if(x == rz){
                        target_ << quad.op << " " << rz << ", " << y << std::endl;
                        a_val_[quad.arg1].erase(rz);
                    }else{
                        std::string rx = GetReg_(j);
                        target_ << "mov " << rz << ", " << x << std::endl;
                        target_ << quad.op << " " << rz << ", " << y << std::endl;
                    }
                    if(y == rz){
                        a_val_[quad.arg2].erase(rz);
                    }
                    r_val_[rz] = {quad.result};
                    a_val_[quad.result] = {rz};
                    RelReg_(quad.arg1);
                    RelReg_(quad.arg2);
                    // quad.result is temp, offset == -1 and live, assign offset
                    if(quad.result[0] == 'T' && quad.result[1] != 'B'){
                        auto it = std::find_if(symbol_table_.begin(), symbol_table_.end(), [&](SymbolTableEntry &entry){
                            return entry.name == quad.result;
                        });
                        if(it != symbol_table_.end() && it->offset == -1 && it->live){
                            it->offset = offset_;
                            offset_ += 4 + it->type * 4;
                        }
                    }
                }else if(quad.op == "R"){
                    target_ << "jmp ?read(" << quad.result << ")" << std::endl;
                }else if(quad.op == "W"){
                    target_ << "jmp ?write(" << quad.result << ")" << std::endl;
                }
                // 更新use live
                UpdLiveUse_(j);
            }
            for(auto &entry : symbol_table_){
                if(entry.live && a_val_[entry.name].find(entry.name) == a_val_[entry.name].end()){
                    std::string ra = *a_val_[entry.name].begin();
                    target_ << "mov " << GetMem_(entry.name) << ", " << ra << std::endl;
                }
            }
            auto quad = quadruples_[block.end];
            if(quad.op == "j"){
                target_ << "jmp ?" << quadruples_[block.end].result << std::endl;
            }else if(quad.op[0] == 'j'){
                std::string x = GetVal_(quad.arg1);
                std::string y = GetVal_(quad.arg2);
                if(x[0] != 'R'){
                    std::string rx = GetReg_(block.end);
                    target_ << "mov " << rx << ", " << x << std::endl;
                    x = rx;
                }
                target_ << "cmp " << x << ", " << y << " and " << quad.op << " ?" << quad.result << std::endl;
            }else if(quad.op == "jnz"){
                std::string x = GetVal_(quad.arg1);
                if(x[0] != 'R'){
                    std::string rx = GetReg_(block.end);
                    target_ << "mov " << rx << ", " << x << std::endl;
                    x = rx;
                }
                target_ << "cmp " << x << ", 0" << std::endl;
                target_ << "jne ?" << quad.result << std::endl;
            }else if(quad.op == "End"){
                target_ << "halt" << std::endl;
            }
            for(auto &reg : registers_){
                r_val_[reg.name] = {};
            }
            for(auto &entry : symbol_table_){
                a_val_[entry.name] = {};
            }
        }
        return target_.str();
    }

    bool CheckTheta_(std::string theta){
        return theta != "End" && theta != "R" && theta != "W" && theta[0] != 'j';
    }

    // 输入：name - 变量名
    // 输出：返回变量所在的寄存器名或内存地址
    // 作用：return R \in Aval(x) ? R : [ebp-x.offset] 
    std::string GetVal_(std::string name){
        for(auto it : a_val_[name]){
            if(it == name){
                continue;
            }
            return it;
        }
        return GetMem_(name);
    }

    // 输入：name - 变量名
    // 输出：返回变量的内存地址
    // 作用：返回变量的内存地址
    std::string GetMem_(std::string name){
        auto it = std::find_if(symbol_table_.begin(), symbol_table_.end(), [&](SymbolTableEntry &entry){
            return entry.name == name;
        });
        assert(it != symbol_table_.end());
        return "[ebp-" + std::to_string(it->offset) + "]";
    }

    // 输入：variable - 变量名, quadIndex - 四元式编号
    // 输出：变量所在的寄存器名
    // 作用：获取变量所在的寄存器，如果没有分配寄存器，则进行分配
    std::string GetReg_(int quadindex){
        auto quad = quadruples_[quadindex];
        if(CheckTheta_(quad.op)){
            for(auto reg : a_val_[quad.arg1]){
                if(reg[0] == 'R' && reg == quad.arg1 && r_val_[reg].size() == 1 && (quad.arg1 == quad.result || !quadruples_[quadindex].live_arg1)){
                    return reg;
                }
            }
        }
        for(auto reg : registers_){
            if(r_val_[reg.name].empty()){
                return reg.name;
            }
        }
        std::string Ri;
        bool ok = false;
        for(auto reg : registers_){
            bool flag = true;
            for(auto a : r_val_[reg.name]){
                auto it = std::find_if(symbol_table_.begin(), symbol_table_.end(), [&](SymbolTableEntry &entry){
                    return entry.name == a;
                });
                if(it->live && !a_val_[a].count(a)){
                    flag = false;
                    break;
                }
            }
            if(flag){
                Ri = reg.name;
                ok = true;
                break;
            }
        }
        if(!ok){
            // Ri = (Rj\in R) argmax (a\in Rval(Rj)) min a.use
            int min_use = INT_MAX, min_reg = -1;
            for(int i = 0; i < registers_.size(); i ++){
                for(auto a : r_val_[registers_[i].name]){
                    auto it = std::find_if(symbol_table_.begin(), symbol_table_.end(), [&](SymbolTableEntry &entry){
                        return entry.name == a;
                    });
                    if(it->use != -1 && it->use < min_use){
                        min_use = it->use;
                        min_reg = i;
                    }
                }
            }
        }
        for(auto a : r_val_[Ri]){
            if(a != quad.result && !a_val_[a].count(a)){
                target_ << "mov " << GetMem_(a) << ", " << Ri << std::endl;
            }
            if(a == quad.arg1 || (a == quad.arg2 && r_val_[Ri].count(quad.arg1))){
                a_val_[a] = {a, Ri};
            }else{
                a_val_[a] = {a};
            }
            r_val_[Ri].erase(a);
        }
        return Ri;
    }

    // 输入：reg - 寄存器名, quadIndex - 四元式编号
    // 输出：无
    // 作用：释放寄存器
    void RelReg_(std::string var){
        if(var[1] == 'B'){
            return;
        }
        for(auto reg : a_val_[var]){
            if(reg[0] == 'R'){
                r_val_[reg].erase(var);
                a_val_[var].erase(reg);
            }
        }
    }

    // 输入：sym - 临时变量名 Tnum_i or Tnum_d
    // 输出：无
    // 作用：插入符号表
    void InsertSym_(std::string sym){
        // 创建符号表条目
        SymbolTableEntry entry(sym);
        // type
        entry.offset = -1;
        entry.type = sym.back() == 'i' ? 0 : 1;
    }

    void UpdLiveUse_(int quadindex){
        auto quad = quadruples_[quadindex];
        for(const auto &t : {std::make_tuple(quad.arg1, quad.use_arg1, quad.live_arg1), 
                            std::make_tuple(quad.arg2, quad.use_arg2, quad.live_arg2), 
                            std::make_tuple(quad.result, quad.use_result, quad.live_result)}){
            auto& [name, u, l] = t;
            auto it = std::find_if(symbol_table_.begin(), symbol_table_.end(), [&](SymbolTableEntry &entry){
                return entry.name == name;
            });
            it->use = u;
            it->live = l;
        }
    }

    // 输入：无
    // 输出：打印寄存器描述符和地址描述符的内容
    // 作用：打印寄存器描述符和地址描述符的内容，用于调试
    void Print_(){
        std::cout << "Registers:" << std::endl;
        for(auto &reg : registers_){
            std::cout << reg.name << ": ";
            for(auto &val : r_val_[reg.name]){
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "Address:" << std::endl;
        for(auto &entry : symbol_table_){
            std::cout << entry.name << ": ";
            for(auto &val : a_val_[entry.name]){
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
    }
};

#endif // QUAD_PROCESSOR_H_