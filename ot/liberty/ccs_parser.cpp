#include <ot/liberty/ccs_parser.hpp>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <cstdlib>

namespace ot {
namespace ccs {

// Parse the main output_current block
std::vector<Lut> CCSParser::parse_output_current_block(
    token_iterator start,
    token_iterator end,
    token_iterator& next_pos,
    const std::unordered_map<std::string, LutTemplate>& lut_templates
) {
    std::vector<Lut> results;

    auto current = start;

    // Skip the keyword ("output_current_rise" or "output_current_fall")
    if (current == end) {
        next_pos = end;
        return results;
    }
    ++current;

    // Skip empty parentheses if present
    if (current != end && *current == "(") {
        ++current;
        if (current != end && *current == ")") {
            ++current;
        }
    }

    // Find the opening brace
    current = find_token(current, end, "{");
    if (current == end) {
        next_pos = end;
        return results;
    }

    // Find matching closing brace
    auto [block_start, block_end] = find_block_bounds(current, end);
    if (block_start == end || block_end == end) {
        next_pos = end;
        return results;
    }

    // Parse each vector within the block
    auto vec_pos = block_start;
    ++vec_pos; // Skip opening brace

    while (vec_pos < block_end) {
        if (*vec_pos == "vector") {
            token_iterator vec_next;
            auto vec = parse_ccs_vector(vec_pos, block_end, vec_next);
            auto lut = convert_ccs_vector_to_lut(vec, lut_templates);
            results.push_back(std::move(lut));
            vec_pos = vec_next;
        } else {
            ++vec_pos;
        }
    }

    next_pos = block_end;
    if (next_pos != end && *next_pos == "}") {
        ++next_pos;
    }

    return results;
}

// Parse a single CCS vector
CCSVector CCSParser::parse_ccs_vector(
    token_iterator start,
    token_iterator end,
    token_iterator& next_pos
) {
    CCSVector vec;

    auto current = start;

    // Expect "vector"
    if (current == end || *current != "vector") {
        next_pos = end;
        return vec;
    }
    ++current;

    // Parse template name in parentheses
    if (current != end && *current == "(") {
        ++current;
        if (current != end) {
            vec.template_name = *current;
            ++current;
        }
        if (current != end && *current == ")") {
            ++current;
        }
    }

    // Find the opening brace for the vector block
    current = find_token(current, end, "{");
    if (current == end) {
        next_pos = end;
        return vec;
    }

    auto [block_start, block_end] = find_block_bounds(current, end);
    if (block_start == end || block_end == end) {
        next_pos = end;
        return vec;
    }

    // Parse contents of vector block
    current = block_start;
    ++current; // Skip opening brace

    while (current < block_end) {
        if (*current == "reference_time") {
            ++current; // Skip "reference_time"
            if (current != end && *current == ":") ++current;
            if (current != end) {
                vec.reference_time = std::strtof(current->c_str(), nullptr);
                ++current;
            }
        }
        else if (*current == "index_1") {
            ++current;
            current = parse_parentheses_values(current, block_end, vec.index_1);
        }
        else if (*current == "index_2") {
            ++current;
            current = parse_parentheses_values(current, block_end, vec.index_2);
        }
        else if (*current == "index_3") {
            ++current;
            vec.index_3 = std::vector<float>();
            current = parse_parentheses_values(current, block_end, vec.index_3.value());
        }
        else if (*current == "index_4") {
            ++current;
            vec.index_4 = std::vector<float>();
            current = parse_parentheses_values(current, block_end, vec.index_4.value());
        }
        else if (*current == "values") {
            ++current;
            if (current != end && *current == "(") {
                ++current;
                // Collect all tokens until closing parenthesis
                // This handles multi-line values
                std::string combined_values;
                while (current != block_end && *current != ")") {
                    std::string token = *current;
                    // Skip backslash tokens (line continuation)
                    if (token == "\\") {
                        ++current;
                        continue;
                    }
                    // Remove quotes if present
                    if (!token.empty() && token[0] == '"') {
                        token = token.substr(1);
                    }
                    if (!token.empty() && token[token.length() - 1] == '"') {
                        token = token.substr(0, token.length() - 1);
                    }
                    // Add comma separator if we already have values
                    if (!combined_values.empty() && !token.empty()) {
                        combined_values += ", ";
                    }
                    combined_values += token;
                    ++current;
                }
                vec.values = parse_value_string(combined_values);
                if (current != end && *current == ")") ++current;
            }
        }
        else {
            ++current;
        }
    }

    next_pos = block_end;
    if (next_pos != end && *next_pos == "}") {
        ++next_pos;
    }

    return vec;
}

// Convert CCSVector to Lut
Lut CCSParser::convert_ccs_vector_to_lut(
    const CCSVector& vec,
    const std::unordered_map<std::string, LutTemplate>& templates
) {
    Lut lut;

    // Set template name
    lut.name = vec.template_name;

    // Look up template
    auto it = templates.find(vec.template_name);
    if (it != templates.end()) {
        lut.lut_template = &(it->second);
    }

    // Set indices
    lut.indices1 = vec.index_1;
    lut.indices2 = vec.index_2;

    if (vec.index_3.has_value()) {
        lut.indices3 = vec.index_3.value();
    }

    if (vec.index_4.has_value()) {
        lut.indices4 = vec.index_4.value();
    }

    // Set values
    lut.table = vec.values;

    // Note: reference_time is not stored in standard Lut structure
    // This could be added as an extension if needed

    return lut;
}

// Helper: Find block bounds
std::pair<CCSParser::token_iterator, CCSParser::token_iterator>
CCSParser::find_block_bounds(token_iterator start, token_iterator end) {
    auto open = find_token(start, end, "{");
    if (open == end) {
        return {end, end};
    }

    int brace_count = 1;
    auto close = open;
    ++close;

    while (close != end && brace_count > 0) {
        if (*close == "{") {
            ++brace_count;
        } else if (*close == "}") {
            --brace_count;
        }
        if (brace_count > 0) {
            ++close;
        }
    }

    return {open, close};
}

// Helper: Parse values in parentheses
CCSParser::token_iterator CCSParser::parse_parentheses_values(
    token_iterator start,
    token_iterator end,
    std::vector<float>& values
) {
    auto current = start;

    // Find opening parenthesis
    while (current != end && *current != "(") {
        ++current;
    }

    if (current == end) {
        return current;
    }

    ++current; // Skip opening parenthesis

    // Collect all tokens until closing parenthesis
    std::string combined_values;
    while (current != end && *current != ")") {
        std::string token = *current;
        // Skip backslash tokens (line continuation)
        if (token == "\\") {
            ++current;
            continue;
        }
        // Remove quotes if present
        if (!token.empty() && token[0] == '"') {
            token = token.substr(1);
        }
        if (!token.empty() && token[token.length() - 1] == '"') {
            token = token.substr(0, token.length() - 1);
        }
        // Add comma separator if we already have values
        if (!combined_values.empty() && !token.empty()) {
            combined_values += ", ";
        }
        combined_values += token;
        ++current;
    }

    values = parse_value_string(combined_values);

    if (current != end && *current == ")") {
        ++current;
    }

    return current;
}

// Helper: Parse comma-separated values from a string
std::vector<float> CCSParser::parse_value_string(const std::string& str) {
    std::vector<float> values;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, ',')) {
        // Trim whitespace
        size_t first = item.find_first_not_of(" \t\n\r");
        size_t last = item.find_last_not_of(" \t\n\r");

        if (first != std::string::npos && last != std::string::npos) {
            std::string trimmed = item.substr(first, last - first + 1);
            if (!trimmed.empty()) {
                values.push_back(std::strtof(trimmed.c_str(), nullptr));
            }
        }
    }

    return values;
}

// Helper: Find a specific token
CCSParser::token_iterator CCSParser::find_token(
    token_iterator start,
    token_iterator end,
    const std::string& token
) {
    return std::find(start, end, token);
}

// Helper: Skip a braced block
CCSParser::token_iterator CCSParser::skip_braced_block(
    token_iterator start,
    token_iterator end
) {
    if (start == end || *start != "{") {
        return start;
    }

    int brace_count = 1;
    auto current = start;
    ++current;

    while (current != end && brace_count > 0) {
        if (*current == "{") {
            ++brace_count;
        } else if (*current == "}") {
            --brace_count;
        }
        ++current;
    }

    return current;
}

} // namespace ccs
} // namespace ot