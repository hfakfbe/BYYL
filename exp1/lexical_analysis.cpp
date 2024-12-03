#include "lexical_analysis.h"

// Class State  
State::State(int id, int type, int attr)
{
    this->id = id;
    this->type = type;
    this->attr = attr;
}

int State::get_id() const
{
    return this->id;
}

int State::get_type() const
{
    return this->type;
}

int State::get_attr() const
{
    return this->attr;
}

bool State::operator==(const State &other) const
{
    return id == other.id;
}

bool State::operator<(const State &other) const
{
    return id < other.id;
}
  
// Class Symbol  
Symbol::Symbol(std::string value) : value{value} {}

Symbol::Symbol(char value) : value{value} {}

std::string Symbol::get_value() const
{
    return this->value;
}  

bool Symbol::operator==(const Symbol &other) const
{
    return value == other.value;
}

bool Symbol::operator<(const Symbol &other) const
{
    return value < other.value;
}
  
/**
 * @brief Constructor for the DFA (Deterministic Finite Automaton) class.
 * 
 * Initializes a DFA with a set of states, symbols, and a transition function.
 * Validates the input to ensure the DFA is correctly defined.
 * 
 * @param states A vector of State objects representing the states of the DFA.
 *               The first state must be of type START, and no other state should be of type START.
 *               All states must be unique.
 * @param symbols A vector of Symbol objects representing the input symbols the DFA can process.
 *                All symbols must be unique.
 * @param transitions A 2D vector of State objects representing the transition function.
 *                    transitions[i][j] represents the state transitioned to from state i on input symbol j.
 *                    The outer vector must have the same size as the number of states.
 *                    The inner vectors must have the same size as the number of symbols.
 *                    All transitions must be valid states within the DFA.
 * 
 * @throws std::invalid_argument If any of the validation checks fail, an invalid_argument exception is thrown with an appropriate error message.
 */
DFA::DFA(std::vector<State> states, std::vector<Symbol> symbols, std::vector<std::vector<State>> transitions)
{
    if(states.empty()){
        throw std::invalid_argument("DFA must have at least 1 states");
    }
    if(states[0].get_type() != State::START){
        throw std::invalid_argument("DFA 0 state must be of type START");
    }
    for(int i = 3; i < states.size(); i++){
        if(states[i].get_type() == State::START){
            throw std::invalid_argument("DFA >= 1 state must be of type GENERAL");
        }
    }
    for(int i = 0; i < states.size(); i++){
        if(this->states.count(states[i])){
            throw std::invalid_argument("DFA must have unique states");
        }else{
            this->states[states[i]] = i;
        }
    }
    for(int i = 0; i < symbols.size(); i++){
        if(this->symbols.count(symbols[i])){
            throw std::invalid_argument("DFA must have unique symbols");
        }else{
            this->symbols[symbols[i]] = i;
        }
    }
    if(transitions.size() != states.size()){
        throw std::invalid_argument("DFA transition function must have the same size as number of states");
    }
    for(int i = 0; i < transitions.size(); i++){
        if(transitions[i].size() != symbols.size()){
            throw std::invalid_argument("DFA transition function must have the same size as number of symbols");
        }
        for(int j = 0; j < transitions[i].size(); j++){
            if(!this->states.count(transitions[i][j])){
                throw std::invalid_argument("DFA transition function must have only states from DFA");
            }
        }
    }
    this->transition_function = transitions;
    this->current_state = states[0];
}

int DFA::reset()
{
    this->current_state = states[0];
    return 0;
}

int DFA::go_next_state(Symbol symbol)
{
    if(!this->symbols.count(symbol)){
        return -1;
    }
    this->current_state = this->transition_function[this->states[this->current_state]][this->symbols[symbol]];
    return 0;
}

State DFA::get_current_state()
{
    return this->current_state;
}
  
// Class LexicalAnalyzer  
LexicalAnalyzer::LexicalAnalyzer(DFA dfa) : dfa(dfa) {}

int LexicalAnalyzer::set_dfa(DFA)
{
    this->dfa = dfa;
    return OK;
}

/**
 * @brief Analyzes the given input string using a Deterministic Finite Automaton (DFA)
 *        to tokenize and categorize the input symbols.
 *
 * @param input The string to be analyzed by the lexical analyzer.
 * @return An integer indicating the result of the analysis:
 *         - OK: The input was successfully analyzed.
 *         - UNRECOGNIZED_SYMBOL: An unrecognized symbol was encountered in the input.
 *         - UNKNOWN_ERROR: An unknown error occurred during the analysis.
 *         - UNRECOGNIZED_IDENTIFIER: An unrecognized identifier was found in the input.
 *
 * @details The function iterates through the input string using two pointers,
 *          `pcur` (current position) and `pstart` (start position of the current token).
 *          It resets the DFA to its initial state and then processes each character
 *          in the input string.
 *          
 *          For each character, it checks if the DFA can transition to the next state
 *          using the current character as a symbol. If the DFA cannot transition,
 *          it returns UNRECOGNIZED_SYMBOL.
 *          
 *          If the DFA reaches a FAIL state, it records the error code and returns UNKNOWN_ERROR.
 *          
 *          If the DFA reaches an OK state, it checks if the attribute of the state is not IGNORE.
 *          If not, it adds the current token (from `pstart` to `pcur-1`) and its attribute
 *          to the result list. The `pcur` is decremented to re-process the last character
 *          of the token in the next iteration (to ensure no character is skipped).
 *          
 *          If the DFA does not reach an OK state after processing all characters,
 *          it returns UNRECOGNIZED_IDENTIFIER.
 */
int LexicalAnalyzer::analyze(std::string input)
{
    int pcur = 0;
    while(pcur < input.size()){
        std::string scur;
        dfa.reset();
        State state = dfa.get_current_state();
        while(state.get_type() != State::OK && pcur < input.size()){
            if(dfa.go_next_state(Symbol(input[pcur]))){
                return UNRECOGNIZED_SYMBOL;
            }
            if(dfa.get_current_state().get_type() != State::START){
                scur += input[pcur];
            }
            pcur ++;
            state = dfa.get_current_state();
            if(state.get_type() == State::FAIL){
                // std::cout << "ERROR: " << state.get_id() << " " << scur.substr(0, scur.size() - 1) << std::endl;
                error_code = state.get_attr();
                return UNKNOWN_ERROR;
            }
        }
        // std::cout << "id:" << state.get_id() << " str:" << scur.substr(0, scur.size() - 1) << " attr:" << state.get_attr() << std::endl;
        if(state.get_type() == State::OK){
            if(state.get_attr() != IGNORE){
                result.push_back({scur.substr(0, scur.size() - 1), state.get_attr()});
            }
            pcur --;
        }else{
            return UNRECOGNIZED_IDENTIFIER;
        }
    }
    return OK;
}

std::vector<std::pair<std::string, int>> LexicalAnalyzer::get_result()
{
    return this->result;
}

int LexicalAnalyzer::reset()
{
    this->dfa.reset();
    this->result.clear();
    return OK;
}

int LexicalAnalyzer::get_error()
{
    return this->error_code;
}
