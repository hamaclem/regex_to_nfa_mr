#include "parser.hpp"
#include "lexer.hpp"
#include "nfa.hpp"
#include <iostream>
#include <vector>
#include <string>

std::vector<Row> rows = {
    // {1, "1/2/2018 5:30", "ASSAULT", 41.69, -87.66},
    {2, "1/2/2018 5:35", "ROBBERY", 41.10, -87.50},
    {3, "1/2/2018 5:40", "BURGLARY", 41.34, -87.57},
    {4, "1/2/2018 5:45", "ROBBERY", 41.13, -87.55},
    {5, "1/2/2018 5:50", "ASSAULT", 41.25, -87.61},
    {6, "1/2/2018 5:55", "BATTERY", 41.12, -87.51},
    {7, "1/2/2018 6:00", "NARCOTICS", 41.17, -87.59},
    {8, "1/2/2018 6:05", "MOTOR VEHICLE THEFT", 41.11, -87.53},
    {9, "1/2/2018 6:10", "OTHER OFFENCE", 41.18, -87.56}
};

GuardFn wildcard = [] (const std::vector<matchedVar> &buffer, const Row &row) {
    return true;
};
GuardFn guard_R = [] (const std::vector<matchedVar> &buffer, const Row &row) {
    return row.primary_type == "ROBBERY";
};
GuardFn guard_B = [] (const std::vector<matchedVar> &buffer, const Row &row) {
    return row.primary_type == "BATTERY";
};
GuardFn guard_M = [] (const std::vector<matchedVar> &buffer, const Row &row) {
    return row.primary_type == "MOTOR VEHICLE THEFT";
};

GuardFn guard_for_var(char var) {
    switch (var) {
        case 'R': return guard_R;
        case 'B': return guard_B;
        case 'M': return guard_M;
        case 'Z': return wildcard;
        default:  return wildcard;
    }
}

int main() {
    std::string pattern = "RZ*BZ*M";

    Lexer lexer(pattern);
    Parser parser(lexer);
    Node* ast = parser.parse_pattern();
    NFA nfa = build_from_AST(ast);
    delete(ast);

     for (State &state : nfa.states) {
        if (state.out1.type == TransitionType::VAR)
            state.out1.guard = guard_for_var(state.out1.var);
    }

    Simulation sim(nfa);
    bool matched = sim.run(rows);
    std::cout << (matched ? "MATCH FOUND\n" : "NO MATCH\n");

    return 0;
}