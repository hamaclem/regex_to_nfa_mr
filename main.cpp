#include "parser.hpp"
#include "lexer.hpp"
#include "nfa.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm> 
#include <cmath>
#include <iomanip>
#include <ctime>
#include <sstream>

#define SHINY_CYAN "\033[1;38;2;0;255;255m"
#define RESET_COLOR      "\033[0m"

bool after_match_skip_to_next_row = false;

std::vector<Row> rows = {
    {1, "1/2/2018 5:30", 0, "ASSAULT", 41.69, -87.66},
    {2, "1/2/2018 5:35", 0, "ROBBERY", 41.10, -87.50},
    {3, "1/2/2018 5:40", 0, "BURGLARY", 41.34, -87.57},
    {4, "1/2/2018 5:45", 0, "ROBBERY", 41.13, -87.55},
    {5, "1/2/2018 5:50", 0, "ASSAULT", 41.25, -87.61},
    {6, "1/2/2018 5:55", 0, "BATTERY", 41.12, -87.51},
    {7, "1/2/2018 6:00", 0, "NARCOTICS", 41.17, -87.59},
    {8, "1/2/2018 6:05", 0, "MOTOR VEHICLE THEFT", 41.11, -87.53},
    {9, "1/2/2018 6:10", 0, "OTHER OFFENCE", 41.18, -87.56},
    {10, "1/2/2018 6:05", 0, "MOTOR VEHICLE THEFT", 41.11, -87.53},
    {11, "1/2/2018 5:30", 0, "ASSAULT", 41.69, -87.66},
    {12, "1/2/2018 5:35", 0, "ROBBERY", 41.10, -87.50},
    {13, "1/2/2018 5:40", 0, "BURGLARY", 41.34, -87.57},
    {14, "1/2/2018 5:45", 0, "ROBBERY", 41.13, -87.55},
    {15, "1/2/2018 5:50", 0, "ASSAULT", 41.25, -87.61},
    {16, "1/2/2018 5:55", 0, "BATTERY", 41.12, -87.51},
    {17, "1/2/2018 6:00", 0, "NARCOTICS", 41.17, -87.59},
    {18, "1/2/2018 6:05", 0, "MOTOR VEHICLE THEFT", 41.11, -87.53}
};

std::time_t parse_date(std::string dateString) {
    std::tm time = {};
    std::istringstream ss(dateString);

    ss >> std::get_time(&time, "%d/%m/%Y %H:%M");

    std::time_t date = std::mktime(&time);

    return date;
}

bool in_range(double a, double b, double range) {
    return std::abs(a - b) <= range + 1e-5;
}

const Row& get_binding(const std::vector<matchedVar> &buffer, char var) {
    auto binding = std::find_if(buffer.begin(), buffer.end(),
        [var](const matchedVar &matchedVar) {
            return matchedVar.var == var;
        });

    return *binding->row;
}

GuardFn wildcard = [] (const std::vector<matchedVar> &buffer, const Row &row) {
    return true;
};

GuardFn guard_R = [] (const std::vector<matchedVar> &buffer, const Row &R) {
    return R.primary_type == "ROBBERY";
};

GuardFn guard_B = [] (const std::vector<matchedVar> &buffer, const Row &B) {
    if (B.primary_type != "BATTERY") {
        return false;
    };
    const Row &R = get_binding(buffer, 'R');

    bool lon_ok = in_range(B.lon, R.lon, 0.05);
    bool lat_ok = in_range(B.lat, R.lat, 0.02);

    return lon_ok && lat_ok;
};

GuardFn guard_M = [] (const std::vector<matchedVar> &buffer, const Row &M) {
    if (M.primary_type != "MOTOR VEHICLE THEFT") {
        return false;
    };
    const Row &R = get_binding(buffer, 'R');
    bool lon_ok = in_range(M.lon, R.lon, 0.05);
    bool lat_ok = in_range(M.lat, R.lat, 0.02);

    bool within = std::abs(M.datetime - R.datetime) <= 30 * 60;

    return lon_ok && lat_ok && within;
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
    for (Row &row : rows) {
        auto date = parse_date(row.datetime_str);
        row.datetime = date;
    }

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

    while(!rows.empty()) {
        std::cout << SHINY_CYAN << "Starting from ROW " << rows.begin()->id << RESET_COLOR << "\n";
        Simulation sim(nfa);
        sim.find_matches(sim, rows, after_match_skip_to_next_row);
    }
  
    return 0;
}
