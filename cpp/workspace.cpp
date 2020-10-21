#include <iostream>
#include <sstream>

#include "lib/format.h"
#include "lib/lyndon.h"
#include "lib/polylog.h"
#include "lib/profiler.h"
#include "lib/shuffle.h"


template<typename ParamT>
std::string format_expr(const std::string& title, const Linear<ParamT>& expr) {
  std::ostringstream os;
  if (expr.main().empty()) {
    os << title << " - " << expr.annotations().size() << " terms\n";
    os << expr.annotations();
  } else {
    os << title << " - " << expr.main().size() << " terms\n";
    os << expr.main();
  }
  return os.str();
}


int main() {
  // std::cout << list_to_string(shuffle_product(IntWord{1,2}, IntWord{3,4})) << "\n";
  // std::cout << "lynd fact:\n" << list_to_string(lyndon_factorize(IntWord{1,0,1,1,0,0,1})) << "\n";
  // auto expr = Li(3, {1,2,3,4});

  // std::cout << tensor_product(D(1, 2) + 2*D(3, 4), -D(5, 6) + 3*D(7, 8)) << "\n";
  // std::cout << str_join(shuffle_product({
  //   IntWord{0,1,0},
  //   IntWord{1,0},
  //   IntWord{1},
  // }), "\n") << "\n";

  Profiler profiler;
  auto expr = Li(6, {1,2,3,4,5,6,7,8});
  // auto expr = Li(2, {1,2,3,4,5,6});
  profiler.finish("expr");
  std::cout << "Num terms before Lyndon = " << expr.main().size() << "\n";
  //auto lyndon = to_lyndon_basis(expr);
  //profiler.finish("lyndon");
  //std::cout << "Num terms after Lyndon = " << lyndon.main().size() << "\n";
  // std::cout << format_expr("Before Lyndon", expr) << "\n";
  // std::cout << format_expr("After Lyndon", lyndon) << "\n";
}
