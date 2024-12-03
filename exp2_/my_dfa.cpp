#include "my_dfa.h"

MakeDFA::MakeDFA(int num_symbols)
{
    adj_keywords.resize(1);
    adj_operators.resize(1);    
}

/**
 * @brief Inserts a new keyword into the Deterministic Finite Automaton (DFA) represented by the adjacency list.
 *        This function adds the keyword to the DFA by traversing through the adjacency list and expanding it
 *        as necessary to accommodate the new keyword. It also associates an attribute with the final state
 *        of the keyword.
 *
 * @param keyword The keyword string to be inserted into the DFA.
 * @param attr    An integer attribute associated with the final state of the inserted keyword.
 * @return Always returns 0, indicating success. This function does not provide a direct way to indicate
 *         failure as it assumes valid inputs and dynamically expands the DFA structure.
 *
 * @note This function modifies the internal DFA structure (`adj_keywords` and `attr_keywords`) to include
 *       the new keyword and its associated attribute. `adj_keywords` is a vector of maps, where each map
 *       represents the transitions from a state to other states based on the input character. `attr_keywords`
 *       maps the final state of each keyword to its attribute.
 */
int MakeDFA::insert_keyword(std::string keyword, int attr)
{
    int tot = adj_keywords.size();
    int cur = 0;
    for(int i = 0; i < keyword.size(); ++ i){
        if(adj_keywords[cur].count(keyword[i])){
            cur = adj_keywords[cur][keyword[i]];
        }else{
            adj_keywords[cur][keyword[i]] = tot;
            adj_keywords.push_back({});
            cur = tot ++;
        }
    }
    // std::cerr << keyword << " " << cur << std::endl;
    attr_keywords[cur] = attr;
    return 0;
}

/**
 * @brief Inserts a new operator into the Deterministic Finite Automaton (DFA) representation.
 *
 * This function inserts a new operator into the DFA, represented as a tree structure in `adj_operators`.
 * The tree is built incrementally as each character of the operator string `op` is processed.
 * If a path in the tree exists for the current character, the traversal continues along that path.
 * Otherwise, a new node is created and added to the tree, and the traversal continues to this new node.
 * The `attr` parameter is associated with the final node in the path representing the full operator.
 *
 * @param op The string representing the operator to be inserted.
 * @param attr An integer attribute associated with the operator.
 * @return Always returns 0, indicating successful insertion.
 * 
 * @note The DFA representation is maintained in two containers:
 *       - `adj_operators`: A vector of maps representing the adjacency list of the DFA tree.
 *                          Each element in the vector is a map where keys are characters and values are indices to other elements in the vector.
 *       - `attr_operators`: A map that associates attributes (integers) with specific nodes in the DFA tree.
 */
int MakeDFA::insert_operator(std::string op, int attr)
{
    int tot = adj_operators.size();
    int cur = 0;
    for(int i = 0; i < op.size(); ++ i){
        if(adj_operators[cur].count(op[i])){
            cur = adj_operators[cur][op[i]];
        }else{
            adj_operators[cur][op[i]] = tot;
            adj_operators.push_back({});
            cur = tot ++;
            // std::cerr << op[i] << " " << cur << std::endl;
        }
    }
    attr_operators[cur] = attr;
    return 0;
}

DFA MakeDFA::make_dfa()
{
    // init keywords and operators, build a trie tree
    int ok_id = 0;
    for(auto kw : keywords){
        insert_keyword(std::string(kw), ok_id ++);
    }
    for(auto op : operators){
        insert_operator(std::string(op), ok_id ++);
    }

    // count states
    int num_states = 
        1 +                             // start
        adj_keywords.size() - 1 +       // keyword_state
        attr_keywords.size() +          // keyword_ok
        1 +                             // identifier_state
        1 +                             // identifier_ok
        adj_operators.size() - 1 +      // operator_state
        attr_operators.size() +         // operator_ok
        1 +                             // err4
        9 +                             // number_state (including 2 oks and 3 errs)
        6;                              // note_state (including 2 oks)
    int state_keyword_ok = 1 + adj_keywords.size() - 1;
    int state_identifier = state_keyword_ok + attr_keywords.size();
    int state_identifier_ok = state_identifier + 1;
    int state_operator = state_identifier_ok + 1;
    int state_operator_ok = state_operator + adj_operators.size() - 1;
    int state_err4 = state_operator_ok + attr_operators.size();
    int state_number = state_err4 + 1;
    int state_note = state_number + 9;

    // count symbols
    int num_symbols = 
        alphabet.size() + 
        numbers.size() + 
        1 + 
        operators_characters.size() + 
        empty_characters.size() + 
        undefined_characters.size();
    int symbol_number = alphabet.size();
    int symbol_dot = symbol_number + numbers.size();
    int symbol_operator_characters = symbol_dot + 1;
    int symbol_empty = symbol_operator_characters + operators_characters.size();
    int symbol_undefined = symbol_empty + empty_characters.size();

    // define dfa conponents
    std::vector<State> states;
    std::vector<Symbol> symbols;
    std::vector<std::vector<int>> transition(num_states, std::vector<int>(num_symbols, -1));

    // initial states
    states.push_back(State(0, State::START));
    for(int i = 0; i < adj_keywords.size() - 1; i ++){
        states.push_back(State(1 + i));
    }
    for(int i = 0; i < attr_keywords.size(); i ++){
        states.push_back(State(state_keyword_ok + i, State::OK));
    }
    states.push_back(State(state_identifier));
    states.push_back(State(state_identifier_ok, State::OK, ok_id ++));
    for(int i = 0; i < adj_operators.size() - 1; i ++){
        states.push_back(State(state_operator + i));
    }
    for(int i = 0; i < attr_operators.size(); i ++){
        states.push_back(State(state_operator_ok + i, State::OK));
    }
    states.push_back(State(state_err4, State::FAIL, 4));
    for(int i = 0; i < 9; ++ i){
        states.push_back(State(state_number + i));
    }
    for(int i = 0; i < 6; ++ i){
        states.push_back(State(state_note + i));
    }

    // initial symbols
    for(auto ch : alphabet){
        symbols.push_back(Symbol(std::string(ch)));
    }
    for(auto ch : numbers){
        symbols.push_back(Symbol(std::string(ch)));
    }
    symbols.push_back(Symbol(std::string(dot_character)));
    for(auto ch : operators_characters){
        symbols.push_back(Symbol(std::string(ch)));
    }
    for(auto ch : empty_characters){
        symbols.push_back(Symbol(std::string(ch)));
    }
    for(auto ch : undefined_characters){
        symbols.push_back(Symbol(std::string(ch)));
    }

    // mapping symbols to indexes
    std::map<std::string, int> symbols_map;
    for(int i = 0; i < symbols.size(); ++ i){
        symbols_map[symbols[i].get_value()] = i;
    }

    // start part transition
    // a~zA~Z
    for(int i = 0; i < symbol_number; i ++){
        transition[0][i] = state_identifier;
    }
    for(auto [ch, next] : adj_keywords[0]){
        transition[0][symbols_map[std::string(1, ch)]] = next;
    }
    // 0~9
    transition[0][symbol_number] = state_number + 2;
    for(int i = symbol_number + 1; i < symbol_operator_characters; i ++){
        transition[0][i] = state_number;
    }
    // . , err2
    transition[0][symbols_map["."]] = state_number + 4;
    // operator characters
    for(int i = symbol_operator_characters; i < num_symbols; i ++){
        transition[0][i] = state_err4;
    }
    for(auto [ch, next] : adj_operators[0]){
        transition[0][symbols_map[std::string(1, ch)]] = state_operator + next - 1;
    }
    // empty characters
    for(int i = symbol_empty; i < num_symbols; i ++){
        transition[0][i] = 0;
    }

    // words part transition
    for(int i = 1, keyword_cnt = 0; i < adj_keywords.size(); i ++){
        // initially set identifier
        for(int j = 0; j < symbol_dot; j ++){
            transition[i][j] = state_identifier;
        }
        for(int j = symbol_dot; j < num_symbols; j ++){
            transition[i][j] = state_identifier_ok;
        }
        // set keyword next
        for(auto [ch, next] : adj_keywords[i]){
            transition[i][symbols_map[std::string(1, ch)]] = next;
        }
        // set ok
        if(attr_keywords.count(i)){
            auto idx = state_keyword_ok + keyword_cnt;
            keyword_cnt ++;
            states[idx] = State(states[idx].get_id(), State::OK, attr_keywords[i]);
            for(int j = symbol_dot; j < num_symbols; j ++){
                transition[i][j] = idx;
            }
        }else{
            for(int j = symbol_dot; j < symbol_empty; j ++){
                transition[i][j] = state_identifier_ok;
            }
        }
    }
    for(int i = 0; i < symbol_dot; i ++){
        transition[state_identifier][i] = state_identifier;
    }
    for(int i = symbol_dot; i < num_symbols; i ++){
        transition[state_identifier][i] = state_identifier_ok;
    }

    // operators part transition
    for(int i = 1, operator_cnt = 0; i < adj_operators.size(); i ++){
        // set set err
        for(int j = 0; j < num_symbols; j ++){
            transition[i - 1 + state_operator][j] = state_err4;
        }
        // set ok
        if(attr_operators.count(i)){
            auto idx = state_operator_ok + operator_cnt;
            operator_cnt ++;
            states[idx] = State(states[idx].get_id(), State::OK, attr_operators[i]);
            for(int j = 0; j < num_symbols; j ++){
                transition[i - 1 + state_operator][j] = idx;
            }
        }
        // set not err
        for(auto [ch, next] : adj_operators[i]){
            transition[i - 1 + state_operator][symbols_map[std::string(1, ch)]] = state_operator + next - 1;
        }
    }

    // number part transition
    states[state_number + 1] = State(states[state_number + 1].get_id(), State::OK, ok_id ++);
    states[state_number + 3] = State(states[state_number + 3].get_id(), State::FAIL, 3);
    states[state_number + 4] = State(states[state_number + 4].get_id(), State::FAIL, 2);
    states[state_number + 6] = State(states[state_number + 6].get_id(), State::FAIL, 1);
    states[state_number + 7] = State(states[state_number + 7].get_id(), State::OK, ok_id ++);
    // 0~9
    for(auto [s, t] : std::vector<std::pair<int, int>>{{0, 0}, {2, 3}, {5, 8}, {8, 8}}){
        for(int j = symbol_number; j < symbol_operator_characters; j ++){
            transition[state_number + s][j] = state_number + t;
        }
    }
    // .
    for(auto [s, t] : std::vector<std::pair<int, int>>{{0, 5}, {2, 5}, {5, 6}, {8, 6}}){
        transition[state_number + s][symbols_map["."]] = state_number + t;
    }
    // other
    for(auto i : {0, 2, 5, 8}){
        for(int j = 0; j < symbol_number; j ++){
            transition[state_number + i][j] = state_err4;
        }
        int tmp_code = (i < 5 ? 1 : (i == 5 ? 4 : 7));
        for(int j = symbol_operator_characters; j < num_symbols; ++ j){
            transition[state_number + i][j] = state_number + tmp_code;
        }
    }

    // undefined character part transition
    for(int i = 0; i < num_states; i ++){
        for(int j = symbol_undefined; j < num_symbols; j ++){
            transition[i][j] = state_err4;
        }
    }

    // note part transition
    int state_slash = transition[0][symbols_map["/"]];
    states[state_note + 3] = State(states[state_note + 3].get_id(), State::OK, LexicalAnalyzer::IGNORE);
    states[state_note + 5] = State(states[state_note + 5].get_id(), State::OK, LexicalAnalyzer::IGNORE);
    transition[state_slash][symbols_map["*"]] = state_note;
    transition[state_slash][symbols_map["/"]] = state_note + 4;
    for(int i = 0; i < num_symbols; i ++){
        transition[state_note][i] = state_note;
        transition[state_note + 1][i] = state_note;
        transition[state_note + 2][i] = state_note + 3;
        transition[state_note + 4][i] = state_note + 4;
    }
    transition[state_note + 0][symbols_map["*"]] = state_note + 1;
    transition[state_note + 1][symbols_map["*"]] = state_note + 1;
    transition[state_note + 1][symbols_map["/"]] = state_note + 2;
    transition[state_note + 4][symbols_map["\n"]] = state_note + 5;

    std::vector<std::vector<State>> transition_function(num_states, std::vector<State>(num_symbols));
    for(int i = 0; i < num_states; i ++){
        for(int j = 0; j < num_symbols; j ++){
            // std::cout << transition[i][j] << ",\n"[j == num_symbols - 1];
            transition_function[i][j] = states[transition[i][j]];
        }
    }

    return DFA(states, symbols, transition_function);
}