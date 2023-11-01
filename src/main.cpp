#include <iostream>
#include <list>
#include "parser.hpp"
#include "parser/expression.hpp"
#include "traits.hpp"

struct var
    : std::variant<
          int,
          float,
          std::list<var>>
{
};

int main()
{
    //resolve_t<decltype(test)> a = 13;
    auto fn = [](auto p) {
        return std::function<
            traits::invoke_signature_t<
                decltype(p)>>{
            std::move(p)};
    };
    fn(parser::addition);

    std::cout
        << "result:"
        << std::endl
        << parser::addition("5 + 5+5")
        << std::endl;
}