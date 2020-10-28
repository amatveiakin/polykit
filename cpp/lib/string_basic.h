#pragma once

#include <string>


// For generic programming like str_join
inline std::string to_string(int x) { return std::to_string(x); }
inline std::string to_string(std::string s) { return s; }
