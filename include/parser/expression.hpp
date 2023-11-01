#pragma once
#include <variant>
#include <any>
#include <cstdint>
#include <ostream>
#include <charconv>
#include "parser/flow.hpp"
#include "parser/token.hpp"
namespace parser
{
// ==============================================
// Type Declarations
// ==============================================
using Integer = std::int64_t;
using Float = double;
struct List;
struct Addition;
struct Subtraction;
using Expression = std::variant<
    Addition,
    Integer,
    Subtraction
    >;

// ==============================================
// Functions Declarations
// ==============================================
Output<Integer> integer(Input view);
Output<Addition> addition(Input view);
Output<Subtraction> subtraction(Input view);
Output<Expression> number(Input view);
Output<Expression> variable(Input view);
Output<Expression> function(Input view);
Output<Expression> unary_operator(Input view);
Output<Expression> binary_operator(Input view);
Output<Expression> expression(Input view);

// ==============================================
//  Stream Declarations
// ==============================================
std::ostream &operator<<(
    std::ostream &os, const Addition &val);
std::ostream &operator<<(
    std::ostream &os, const Expression &val);
// ===============================================
// Integer
// ===============================================
Output<Integer> integer(Input view)
{
    static auto parser = token("\\d+");
    auto out = parser(view);
    if (not out.has_value())
    {
        return std::nullopt;
    }
    auto [data, next] = out.value();
    auto number = Integer();
    auto [_, e] = std::from_chars(
        data.data(),
        data.data() + data.size(),
        number);
    if (e != std::errc())
    {
        return {};
    }
    return {{number, next}};
}

// ==============================================
// operatores
// ==============================================
struct Addition
{
    Wrapper<Expression> lhs;
    Wrapper<Expression> rhs;
};
struct Subtraction
{
    Wrapper<Expression> lhs;
    Wrapper<Expression> rhs;
};

namespace make
{
template <typename T>
auto binary_operator(const char *symbol)
{
    return [](Input view) -> Output<T> {
        auto seq = sequence(
            select(
                number,
                variable,
                function),
            optional::seperator,
            token("\\+"),
            optional::seperator,
            expression)(view);
        if (not seq.has_value())
        {
            return std::nullopt;
        }
        auto &&[data, next] = seq.value();
        return {{
            {Wrapper{std::get<0>(data)},
             Wrapper{std::get<4>(data)}},
            next,
        }};
    };
}
} // namespace make


Output<Expression> transform_operator(Input view)
{
    auto parse = [](const char *symbol, auto&& result) {
        using Result = std::decay_t<decltype(result)>;
        return [symbol](Input view, Expression& lhs) {
            auto seq = sequence(
                optional::seperator,
                token(symbol),
                optional::seperator,
                expression)(view);
            if (not seq.has_value())
            {
                return Output<Expression>{};
            }
            auto &&[data, next] = seq.value();
            return Output<Expression>{
                {Result{
                     Wrapper{lhs},
                     Wrapper{std::get<3>(data)}},
                 next}};
        };
    };

    return feedforward(
        select(
            number,
            variable,
            function),
        select(
            parse("\\+", Addition{}),
            parse("\\-", Subtraction{})))(view);
}

/*  select(
       parse("\\+"));
    

   auto out = parse_lhs(view);

        if (not aut.has_value())
        {
            return std::nullopt;
        }
        auto &&[data, next] = seq.value();
        return {{
            {Wrapper{std::get<0>(data)},
             Wrapper{std::get<4>(data)}},
            next,
        }};
}*/

Output<Addition> addition(Input view)
{
    static const auto parse =
        make::binary_operator<Addition>("\\+");
    return parse(view);
}

Output<Subtraction> subtraction(Input view)
{
    static const auto parse =
        make::binary_operator<Subtraction>("\\-");
    return parse(view);
}

std::ostream &operator<<(
    std::ostream &os, const Addition &val)
{
    os << val.lhs << "+" << val.rhs;
    return os;
}
// ==============================================
// Expression Groups
// ==============================================
//Output<Expression> binary_operator(Input view)
//{
//    static const auto parse = select(
//        addition,
//        subtraction);
//    return parse(view);
//}

//Output<Expression> unary_operator(Input view)
//{
//  static const auto parse = select(addition);
//}
Output<Expression> number(Input view)
{
    return integer(view);
}
Output<Expression> variable(Input view)
{
    return {};
}
Output<Expression> function(Input view)
{
    return {};
}
// ==============================================
// Expression
// ==============================================
Output<Expression> expression(Input view)
{
    static const auto parse = select(
        addition,
        integer,
        subtraction
    );
    return parse(view);
}
std::ostream &operator<<(
    std::ostream &os, const Expression &val)
{
    return std::visit(
        [&os](auto &v) -> decltype(auto) {
            return os << v;
        },
        val);
}

} // namespace parser