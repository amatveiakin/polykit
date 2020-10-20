#include <iostream>

#include "lib/format.h"
#include "lib/lyndon.h"
#include "lib/polylog.h"
#include "lib/shuffle.h"


int main() {
  // std::cout << list_to_string(shuffle_product(IntWord{1,2}, IntWord{3,4})) << "\n";
  // std::cout << "lynd fact:\n" << list_to_string(lyndon_factorize(IntWord{1,0,1,1,0,0,1})) << "\n";
  auto expr = Li2(1,2,3,4);
  std::cout << "Before Lyndon:\n" << expr << "\n";
  std::cout << "After Lyndon:\n" << to_lyndon_basis(expr) << "\n";
}
