#pragma once

#include <optional>
#include <string>


std::optional<std::string> unicode_subscript(char ch);
std::optional<std::string> unicode_superscript(char ch);

std::optional<std::string> unicode_mathcal(char ch);
std::optional<std::string> unicode_mathbb(char ch);
