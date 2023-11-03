#pragma once
#include <variant>
#include "parser.hpp"

namespace parser
{
// ==============================================
// select
// ==============================================
namespace detail
{
template <typename T, typename... Ts>
using select_t = std::conditional_t<
	(... && std::is_same_v<T, Ts>),
	T, std::variant<T, Ts...>>;
}
template <typename T, typename... Ts>
using select_t = detail::select_t<
	parse_t<T>,
	parse_t<Ts>...>;

template <typename P, typename... Ps>
auto select(P parser, Ps... parsers)
{
	using Result = Output<select_t<P, Ps...>>;
	return std::function<
		traits::invoke_signature_r_t<Result, P>>{
		[=](auto view, auto... args) {
			auto parse = [&](auto &self,
							 auto parser,
							 auto... parsers) {
				auto out = parser(view, args...);
				if (out.has_value())
				{
					return Result{
						{{std::get<0>(out.value())},
						 std::get<1>(out.value())}};
				}
				if constexpr (sizeof...(parsers) > 0)
				{
					return self(self, parsers...);
				}
				else
				{
					return Result{};
				}
			};
			return parse(parse, parser, parsers...);
		}};
}

// ==============================================
// sequence
// ==============================================
template <typename... Ts>
using sequence_t = std::tuple<parse_t<Ts>...>;

template <typename P, typename... Ps>
auto sequence(P parser, Ps... parsers)
{
	return [=](auto view) {
		auto parse = [](auto &self,
						auto view,
						auto parser, auto... parsers) {
			using Sequence = sequence_t<
				decltype(parser), decltype(parsers)...>;

			auto out_0 = parser(view);
			if (not out_0.has_value())
			{
				return Output<Sequence>{};
			}
			auto &&[data_0, view_0] = out_0.value();
			if constexpr (sizeof...(parsers) > 0)
			{
				auto out_1 = self(self, view_0, parsers...);
				if (not out_1.has_value())
				{
					return Output<Sequence>{};
				}
				auto &&[data_1, view_1] = out_1.value();
				return Output<Sequence>{
					std::tuple{
						std::tuple_cat(
							std::tuple{data_0}, data_1),
						view_1}};
			}
			else
			{
				return Output<Sequence>{
					std::tuple{
						std::tuple{data_0}, view_0}};
			}
		};
		return parse(parse, view, parser, parsers...);
	};
}
// ==============================================
// forward
// ==============================================
template <
	typename P,
	typename F, size_t I, size_t... Is>
auto forward(P parser, F update)
{
	using Result = parse_t<P>;
	using Forward = Output<
		std::invoke_result_t<
			F, Input,
			std::tuple_element_t<I, Result>,
			std::tuple_element_t<Is, Result>...>>;
	return [=](auto view) {
		auto out = parser(view);
		if (out.has_value())
		{
			auto &&[data, view] = out.value();
			return update(view, std::get<Is>(data)...);
		}
		return Forward{};
	};
}

template <typename P, typename F>
auto forward(P parser, F update)
{
	using Forward = Output<
		std::invoke_result_t<F, Input, parse_t<P>>>;
	return [=](auto view) {
		auto out = parser(view);
		if (out.has_value())
		{
			auto &&[data, view] = out.value();
			return update(view, data);
		}
		return Forward{};
	};
}

// ==============================================
// feedforward
// ==============================================
template <typename... Ps>
using feedforward_t = traits::last_t<parse_t<Ps>...>;

template <typename P, typename... Ps>
auto feedforward(P parser, Ps... parsers)
{
	using Feedforward =
		Output<feedforward_t<P, Ps...>>;
	return [=](auto view) {
		auto parse_0 = [](auto &self,
						  auto view,
						  auto parser,
						  auto... parsers) -> Feedforward {
			if constexpr (sizeof...(parsers) > 0)
			{
				auto out = parser(view);
				if (out.has_value())
				{
					auto &&[data, view] = out.value();
					return self(self, view, data, parsers...);
				}
				return {};
			}
			else
			{
				return parser(view);
			}
		};
		auto parse_n = [](auto &self,
						  auto view,
						  auto data,
						  auto parser,
						  auto... parsers) -> Feedforward {
			if constexpr (sizeof...(parsers) > 0)
			{
				auto out = parser(view, data);
				if (out.has_value())
				{
					auto &&[data, view] = out.value();
					return self(self, view, data, parsers...);
				}
				return {};
			}
			else
			{
				return parser(view, data);
			}
		};
		return parse_0(parse_n, view, parser, parsers...);
	};
};
} // namespace parser
