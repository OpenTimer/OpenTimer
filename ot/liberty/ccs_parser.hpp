#ifndef OT_LIBERTY_CCS_PARSER_HPP_
#define OT_LIBERTY_CCS_PARSER_HPP_

#include <vector>
#include <string>
#include <unordered_map>
#include <optional>
#include <ot/liberty/lut.hpp>

namespace ot {
namespace ccs {

/**
 * @brief Structure to hold CCS vector data before conversion to Lut
 *
 * CCS vectors have a specific structure with reference_time and
 * up to 4 index dimensions plus values.
 */
struct CCSVector {
    float reference_time = 0.0f;
    std::string template_name;
    std::vector<float> index_1;
    std::vector<float> index_2;
    std::optional<std::vector<float>> index_3;
    std::optional<std::vector<float>> index_4;
    std::vector<float> values;
};

/**
 * @brief CCS-specific parser for output_current blocks
 *
 * This class provides static methods to parse CCS output_current data
 * without modifying the passed iterators, avoiding the corruption issues
 * that occur with the standard Liberty parser.
 */
class CCSParser {
public:
    using token_iterator = std::vector<std::string>::const_iterator;

    /**
     * @brief Parse an output_current_rise or output_current_fall block
     *
     * @param start Iterator pointing to "output_current_rise" or "output_current_fall"
     * @param end End iterator of token stream
     * @param next_pos [out] Iterator position after the parsed block
     * @param lut_templates Template definitions for lookup tables
     * @return Vector of Lut objects containing parsed current data
     */
    static std::vector<Lut> parse_output_current_block(
        token_iterator start,
        token_iterator end,
        token_iterator& next_pos,
        const std::unordered_map<std::string, LutTemplate>& lut_templates
    );

    /**
     * @brief Parse a single CCS vector block
     *
     * @param start Iterator pointing to "vector"
     * @param end End iterator of token stream
     * @param next_pos [out] Iterator position after the parsed vector
     * @return CCSVector containing parsed data
     */
    static CCSVector parse_ccs_vector(
        token_iterator start,
        token_iterator end,
        token_iterator& next_pos
    );

    /**
     * @brief Convert CCSVector to Lut format
     *
     * @param vec CCS vector data
     * @param templates LUT template definitions
     * @return Lut object compatible with OpenTimer's data structures
     */
    static Lut convert_ccs_vector_to_lut(
        const CCSVector& vec,
        const std::unordered_map<std::string, LutTemplate>& templates
    );

private:
    /**
     * @brief Find the bounds of a block delimited by braces
     *
     * @param start Start iterator
     * @param end End iterator
     * @return Pair of iterators: (opening brace, closing brace)
     */
    static std::pair<token_iterator, token_iterator>
        find_block_bounds(token_iterator start, token_iterator end);

    /**
     * @brief Parse values from parentheses (e.g., index values)
     *
     * @param start Iterator pointing to opening parenthesis
     * @param end End iterator
     * @param values [out] Vector to store parsed values
     * @return Iterator position after closing parenthesis
     */
    static token_iterator parse_parentheses_values(
        token_iterator start,
        token_iterator end,
        std::vector<float>& values
    );

    /**
     * @brief Parse a quoted string containing comma-separated values
     *
     * @param str String containing values (may be quoted)
     * @return Vector of parsed float values
     */
    static std::vector<float> parse_value_string(const std::string& str);

    /**
     * @brief Find and skip to a specific token
     *
     * @param start Start iterator
     * @param end End iterator
     * @param token Token to find
     * @return Iterator pointing to token, or end if not found
     */
    static token_iterator find_token(
        token_iterator start,
        token_iterator end,
        const std::string& token
    );

    /**
     * @brief Skip a balanced block of braces
     *
     * @param start Iterator pointing to opening brace
     * @param end End iterator
     * @return Iterator pointing after closing brace
     */
    static token_iterator skip_braced_block(
        token_iterator start,
        token_iterator end
    );
};

} // namespace ccs
} // namespace ot

#endif // OT_LIBERTY_CCS_PARSER_HPP_