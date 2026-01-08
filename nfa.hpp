#ifndef NFA_HPP
#define NFA_HPP

#include "parser.hpp"
#include <vector>
#include <functional>
#include <set>
#include <ctime>

struct Row {
    int id;
    std::string datetime_str;
    time_t datetime;
    std::string primary_type;
    float lat;
    float lon;
};

struct matchedVar {
    char var;
    const Row* row; 
};

enum class TransitionType {
    NONE,   
    VAR,    
    EPSILON   
};

using GuardFn = std::function<bool(const std::vector<matchedVar>&, const Row&)>;

struct Transition {
    TransitionType type;
    int to;
    char var;
    GuardFn guard;

    Transition();
    Transition(TransitionType type, int to, char var = 0, GuardFn guard = GuardFn());
};

struct State {
    int id;

    Transition out1;
    Transition out2;

    State(int id = 0);
};

struct NFA {
    int start;
    int accept;
    
    int next_state_id; 
    std::vector<State> states;

    NFA();
    int new_state();
    void add_transition(int from, const Transition &trans);

    NFA build_eps_NFA();
    NFA build_var_NFA(char var, GuardFn guard = GuardFn());

    NFA build_star_NFA(NFA nfa);
    NFA build_plus_NFA(NFA nfa);
    NFA build_opt_NFA(NFA nfa);

    NFA build_union_NFA(NFA nfa1, NFA nfa2);
    NFA build_concat_NFA(NFA nfa1, NFA nfa2);

    void print() const; 
};

NFA build_from_AST(Node* ast);

struct Run {
    int state;
    std::vector<matchedVar> bindings;

    Run(int state = 0);
};

struct Simulation {
    const NFA &nfa;
    std::vector<Run> currentRuns;
    std::vector<Run> accRuns;

    Simulation(const NFA &nfa);

    void epsilon_closure(std::vector<Run> &currentRuns);
    void print_run(const Run &run);
    bool run(const std::vector<Row> &rows);
    void reset();
};

#endif