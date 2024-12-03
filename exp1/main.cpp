#include "lexical_analysis.h"
#include "my_dfa.h"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    MakeDFA a;
    DFA dfa = a.make_dfa();
    LexicalAnalyzer b(dfa);
    std::string s, t;
    while(std::getline(std::cin, t)){
        s += t + "\n";
    }

    int ret = b.analyze(s);
    
    if(ret == LexicalAnalyzer::UNKNOWN_ERROR){
        auto res = b.get_error();
        std::cout << output_errs[res] << "\n";
    }else if(ret == LexicalAnalyzer::UNRECOGNIZED_SYMBOL){
        std::cout << output_errs[4] << "\n";
    }else{
        auto res = b.get_result();
        for(auto [str, type]: res){
            std::cout << str << " " << output_types[type] << "\n";
        }
    }
    return 0;
}