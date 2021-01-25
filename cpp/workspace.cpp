#include <iostream>
#include <regex>
#include <fstream>
#include <sstream>

#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_split.h"
#include "absl/strings/substitute.h"

#include "lib/algebra.h"
#include "lib/coalgebra.h"
#include "lib/cotheta.h"
#include "lib/format.h"
#include "lib/iterated_integral.h"
#include "lib/lexicographical.h"
#include "lib/lyndon.h"
#include "lib/mystic_algebra.h"
#include "lib/packed.h"
#include "lib/polylog.h"
#include "lib/polylog_cross_ratio.h"
#include "lib/polylog_quadrangle.h"
#include "lib/polylog_via_correlators.h"
#include "lib/profiler.h"
#include "lib/projection.h"
#include "lib/sequence_iteration.h"
#include "lib/shuffle.h"
#include "lib/snowpal.h"
#include "lib/summation.h"
#include "lib/theta.h"


int main(int argc, char *argv[]) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler({});

  ScopedFormatting sf(FormattingConfig()
    .set_formatter(Formatter::unicode)
    .set_rich_text_format(RichTextFormat::console)
    .set_annotation_sorting(AnnotationSorting::length)
  );


  const int num_points = 9;
  const int num_args = num_points / 2 - 1;
  auto source = sum_looped_vec(
    [&](const std::vector<X>& args) {
      return LiQuad(
        num_points / 2 - 1,
        mapped(args, [](X x) { return x.var(); })
      );
    },
    num_points,
    seq_incl(1, num_points - 1)
  );

  auto expr = theta_expr_to_lira_expr_without_products(source.without_annotations());


  // LiraExpr lhs = fully_normalize_ratios(expr);
  // LiraExpr rhs;
  // for (int i = 0; i < 7; ++i) {
  //   const auto args = rotated_vector(seq_incl(1, 7), i);
  //   const auto lira_four = slice(args, 0, 4);
  //   const auto lira_five = concat({args[0]}, slice(args, 3));
  //   for (int j = 0; j < 5; ++j) {
  //     const auto lira_five_subset = removed_index(lira_five, j);
  //     const int sign = neg_one_pow(j);
  //     rhs += sign * LiraExpr::single(LiraParamOnes({Ratio(lira_four), Ratio(lira_five_subset)}));
  //   }
  //   // std::cout << rhs << "\n";
  //   // rhs = {};
  // }
  // rhs = fully_normalize_ratios(rhs);
  // // std::cout << "LHS " << lhs << "\n";
  // // std::cout << "RHS " << rhs << "\n";
  // // std::cout << "Sum " << lhs + rhs << "\n";
  // std::cout << "LHS " << to_lyndon_basis_2(lhs) << "\n";
  // std::cout << "RHS " << to_lyndon_basis_2(rhs) << "\n";
  // std::cout << "Sum " << to_lyndon_basis_2(lhs + rhs) << "\n";


  StringExpr stats;

  auto analyze_snowpal = [&](const Snowpal& snowpal) {
    absl::flat_hash_set<std::array<int, 4>> ratios;
    snowpal.expr().foreach([&](const LiraParamOnes& formal_symbol, int) {
      for (const RatioOrUnity& r : formal_symbol.ratios()) {
        if (!r.is_unity()) {
          ratios.insert(sorted(r.as_ratio().indices()));
        }
      }
    });
    stats.add_to(absl::StrCat("vars ", absl::Dec(ratios.size(), absl::kZeroPad2)), 1);
    if (ratios.size() == num_args) {
    // if (num_args <= ratios.size() && ratios.size() <= num_args + 2) {
      std::cout << snowpal << "\n";
    }
  };



  constexpr char kInvalidInput[] = "Invalid input: ";
  std::cout << "Functional\n" << source.annotations() << "\n";
  while (true) {
    std::vector<std::vector<int>> balls;
    std::unique_ptr<Snowpal> snowpal;
    auto reset_snowpal = [&]() {
      snowpal = absl::make_unique<Snowpal>(expr, num_points);
      for (const auto& b : balls) {
        snowpal->add_ball(b);
      }
    };
    auto add_ball = [&](const std::vector<int>& ball) {
      snowpal->add_ball(ball);
      balls.push_back(ball);
    };
    reset_snowpal();
    std::cout << "Original " << expr << "\n";
    while (true) {
      std::cout << "> ";
      std::string input;
      std::getline(std::cin, input);
      trim(input);
      if (input.empty()) {
        continue;
      } else if (input == "q" || input == "quit") {
        return 0;
      } else if (input == "r" || input == "reset") {
        break;
      } else if (input == "b" || input == "back") {
        if (balls.empty()) {
          std::cout << kInvalidInput << "nothing to remove\n";
          continue;
        }
        balls.pop_back();
        reset_snowpal();
        std::cout << "Reverted " << *snowpal << "\n";
        continue;
      }
      std::vector<int> ball;
      try {
        // for (const auto& var_str : absl::StrSplit(input, " ", absl::SkipEmpty())) {
        //   const int var = std::stoi(std::string(var_str));
        //   if (var < 1 || var > num_points) {
        //     throw std::out_of_range(absl::StrCat("variable index out of range: ", var));
        //   }
        //   ball.push_back(var);
        // }
        for (const char ch : input) {
          if (std::isspace(ch)) {
            continue;
          }
          const int var = std::stoi(std::string(1, ch));
          if (var < 1 || var > num_points) {
            throw std::out_of_range(absl::StrCat("variable index out of range: ", var));
          }
          ball.push_back(var);
        }
      } catch (const std::exception& e) {
        std::cout << kInvalidInput << e.what() << "\n";
        continue;
      }
      try {
        snowpal->add_ball(ball);
        balls.push_back(ball);
        std::cout << "Substituted " << *snowpal << "\n";
      } catch (const IllegalTreeCutException& e) {
        std::cout << kInvalidInput << e.what() << "\n";
        reset_snowpal();
      }
    }
  }
}
