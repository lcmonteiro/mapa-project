#pragma once
#include <optional>
#include <tuple>
#include <vector>
#include <any>
#include <ostream>
#include "parser/statement.hpp"
#include "traits.hpp"

namespace parser
{
// ==============================================
// Base Types
// ==============================================
using Input = std::string_view;
template <typename T>
using Output = std::optional<std::tuple<T, Input>>;

// ==============================================
// Helpers Types
// ==============================================
template <typename T>
using value_t = typename T::value_type;
template <typename T>
using parse_t = std::tuple_element_t<
    0, value_t<traits::invoke_result_t<T>>>;

// ==============================================
// Helpers
// ==============================================
template <typename T>
struct Wrapper
{
    Wrapper() = default;
    Wrapper(Wrapper &&) = default;
    Wrapper(const Wrapper &) = default;

    Wrapper(T &&v) : var{std::move(v)} {}
    Wrapper(const T &v) : var{v} {}
    operator const T &() const
    {
        if (not var.has_value())
        {
            static const T def{};
            return def;
        }
        return *std::any_cast<T>(&var);
    }

  private:
    std::any var;
};
template <class T>
Wrapper(T)->Wrapper<std::decay_t<T>>;

template <typename T>
std::ostream &operator<<(
    std::ostream &os, const Output<T> &out)
{
    if (out.has_value())
    {
        auto &[val, rem] = out.value();
        os << "v= " << val << std::endl;
        os << "r= " << rem << std::endl;
    }
    return os;
}

// ==============================================
// Module
// ==============================================
struct Module
{
    std::string name;
    std::vector<Statement> statements;
};
Output<Module> module(Input view)
{
    return {};
}
// =================================
// Program
// =================================
struct Program
{
    std::vector<Module> modules;
};
Output<Program> program(Input view)
{
    return {};
}
} // namespace parser