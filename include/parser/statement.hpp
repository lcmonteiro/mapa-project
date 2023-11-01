#pragma once
#include <variant>
#include <string>
#include <vector>
namespace parser
{
// ==============================================
// Statement Types
// ==============================================
struct Assignment;
struct Condition;
struct Loop;
using Statement = std::variant<
    Assignment,
    //Condition,
    Loop>;

// ==============================================
// Assignment
// ==============================================
struct Assignment
{
    std::string var;
};

// ==============================================
// Loop
// ==============================================
struct Loop
{
    std::string var;
    std::vector<Statement> statements;
};
} // namespace parser