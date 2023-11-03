#pragma once
#include <regex>
#include "parser.hpp"

namespace parser
{
// ==============================================
// Helpers
// ==============================================
using Token = std::string_view;

// ==============================================
// token
// ==============================================
auto token(const char *pattern)
{
	return [re = std::regex(pattern)](Input view) {
		std::cmatch cm;
		if (not std::regex_search(
				std::cbegin(view),
				std::cend(view), cm, re,
				std::regex_constants::match_continuous))
		{
			return Output<Token>{};
		}
		const auto pos = cm.length(0);
		return Output<Token>{
			{view.substr(0, pos),
			 view.substr(pos)}};
	};
}

// ==============================================
// token
// ==============================================
const auto seperator = token("\\s+");
const auto digits = token("\\d+");
namespace optional
{
const auto seperator = token("\\s*");
}

const auto plus = token("\\+");
const auto minus = token("\\-");
const auto multiplies = token("\\*");
} // namespace parser