#include "file_util.h"

#include <fstream>


std::string get_file_content(const std::string& filename) {
  std::ifstream file(filename);
  return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}
