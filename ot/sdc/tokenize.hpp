#ifndef OT_SDC_TOKENIZER_HPP
#define OT_SDC_TOKENIZER_HPP

#include <ot/headerdef.hpp>
#include <ot/traits.hpp>
#include <ot/utility/utility.hpp>
#include <ot/static/logger.hpp>

namespace ot::sdc {

using token_iter_t = std::vector<std::string>::iterator;

std::vector<std::string> tokenize(const std::string&);

};  // end of namespace ot::sdc -------------------------------------------------------------------

#endif
