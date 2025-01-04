#include "quad_processor.h"

// (R,-,-,TB0)
Quadruple ParseQuad(std::string str){
    std::string op, arg1, arg2, result;
    int id;
    std::string::size_type pos = 0;
    auto next = str.find(',', pos);
    op = str.substr(pos + 1, next - pos - 1);
    pos = next + 1;
    next = str.find(',', pos);
    arg1 = str.substr(pos, next - pos);
    pos = next + 1;
    next = str.find(',', pos);
    arg2 = str.substr(pos, next - pos);
    pos = next + 1;
    next = str.find(')', pos);
    result = str.substr(pos, next - pos);
    return Quadruple(op, arg1, arg2, result);
}

// 0: (R,-,-,TB0)
std::vector<Quadruple> ReadQuadruples() {
    int n;
    std::cin >> n;
    std::vector<Quadruple> quadruples;
    std::string op, arg1, arg2, result;
    for (int i = 0; i < n; i ++) {
        std::string t;
        std::cin >> t >> t;
        quadruples.push_back(ParseQuad(t));
    }
    return quadruples;
}

std::vector<SymbolTableEntry> ReadSymbolTable() {
    int n;
    std::cin >> n;
    std::vector<SymbolTableEntry> symbol_table;
    for (int i = 0; i < n; i ++) {
        std::string name;
        int type, offset;
        std::string value;
        std::cin >> name >> type >> value >> offset;
        symbol_table.push_back(SymbolTableEntry(name));
        symbol_table[i].type = type;
        symbol_table[i].value = value;
        symbol_table[i].offset = offset;
    }
    return symbol_table;
}

int main() {
    auto symbol_table = ReadSymbolTable();
    int junk;
    std::cin >> junk;
    auto quadruples = ReadQuadruples();

    QuadProcessor processor(quadruples, symbol_table);
    auto str = processor.ProcessQuadruples();
    std::cout << str;
    return 0;
}