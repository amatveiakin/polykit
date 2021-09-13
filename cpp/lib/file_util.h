#pragma once

#include <string>


std::string get_file_content(const std::string& filename);
void set_file_content(const std::string& filename, const std::string& content);
