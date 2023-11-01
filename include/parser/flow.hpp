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
        traits::invoke_signature_r_t<Result,P>>{
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
// feedforward
// ==============================================
namespace detail
{
//template <typename R, typename P, typename... Ps>
//struct forward : forward<parse_t<P, R>, Ps...>
//{
//};

//template <typename R, typename P>
//struct forward<R, P>
//{
//    using type = parse_t<P, R>;
//};

//template <typename... Ts>
//using feedforward_t =
//    std::tuple_element_t<
//        sizeof...(Ts) - 1, std::tuple<Ts...>>;

} // namespace detail

//template <typename P, typename... Ps>
//using feedforward_t = typename detail::forward<parse_t<P>, Ps...>::type;

template <typename P, typename... Ps>
auto feedforward(P parser, Ps... parsers)
{
    //using Feedforward = feedforward_t<P, Ps...>;
    return [=](auto view) {
        auto parse_0 = [](auto &self,
                          auto view,
                          auto parser, auto... parsers) {
            if constexpr (sizeof...(parsers) > 0)
            {
                using Result = decltype(
                    self(self,
                         view,
                         std::declval<parse_t<decltype(parser)>>(),
                         parsers...));
                auto out = parser(view);
                if (out.has_value())
                {
                    auto &&[data, view] = out.value();
                    return self(self, view, data, parsers...);
                }
                return Result{};
            }
            else
            {
                return parser(view);
            }
        };
        auto parse_n = [](auto &self,
                          auto view,
                          auto data,
                          auto parser, auto... parsers) {
            if constexpr (sizeof...(parsers) > 0)
            {
                using Result = decltype(
                    self(self,
                         view,
                         std::declval<parse_t<
                             decltype(parser)>>(),
                         parsers...));

                auto out = parser(view, data);
                if (out.has_value())
                {
                    auto &&[data, view] = out.value();
                    return self(self, view, data, parsers...);
                }
                return Result{};
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
