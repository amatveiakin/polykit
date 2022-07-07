#include "lib/snowpal.h"


#if 0  // TODO: Turn this into a test
int main(int argc, char *argv[]) {
  const int num_points = 7;
  const int num_args = num_points / 2 - 1;
  auto source = sum_looped_vec(
    [&](const auto& args) {
      return LiQuad(
        num_points / 2 - 1,
        mapped(args, [](X x) { return x.var(); })
      );
    },
    num_points,
    seq_incl(1, num_points - 1)
  );

  auto expr = theta_expr_to_lira_expr_without_products(source.without_annotations());



  StringExpr stats;

  auto analyze_snowpal = [&](const Snowpal& snowpal) {
    absl::flat_hash_set<std::array<int, 4>> ratios;
    snowpal.expr().foreach([&](const LiraParamOnes& formal_symbol, int) {
      for (const CrossRatioNOrUnity& r : formal_symbol.ratios()) {
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



  // The expression is symmetric w.r.t to rotations, so let's fix one arg.
  // This still doesn't cover all symmetries.

  // const int a = 1;
  // for (int b = a+1; b <= num_points; ++b) {
  //   for (int c = b+1; c <= num_points; ++c) {
  //     for (int d = c+1; d <= num_points; ++d) {
  //       Snowpal snowpal(expr, num_points);
  //       snowpal.add_ball({a, b, c, d});
  //       analyze_snowpal(snowpal);
  //     }
  //   }
  // }

  // const int a = 1;
  // for (int b = a+1; b <= num_points; ++b) {
  //   for (int c = b+1; c <= num_points; ++c) {
  //     for (int d = c+1; d <= num_points; ++d) {
  //       for (int e = 2; e <= num_points; ++e) {
  //         for (int f = e+1; f <= num_points; ++f) {
  //           try {
  //             Snowpal snowpal(expr, num_points);
  //             snowpal.add_ball({a, b, c, d}).add_ball({e, f});
  //             analyze_snowpal(snowpal);
  //           } catch (const IllegalTreeCutException&) {}
  //         }
  //       }
  //     }
  //   }
  // }

  // const int a = 1;
  // for (int b = a+1; b <= num_points; ++b) {
  //   for (int c = b+1; c <= num_points; ++c) {
  //     for (int d = 2; d <= num_points; ++d) {
  //       for (int e = d+1; e <= num_points; ++e) {
  //         for (int f = e+1; f <= num_points; ++f) {
  //           try {
  //             Snowpal snowpal(expr, num_points);
  //             snowpal.add_ball({a, b, c}).add_ball({d, e, f});
  //             analyze_snowpal(snowpal);
  //           } catch (const IllegalTreeCutException&) {}
  //         }
  //       }
  //     }
  //   }
  // }

  // const int a = 1;
  // const int b = 4;
  // for (int b = 2; b <= num_points; ++b) {
  //   for (int c = 2; c <= num_points; ++c) {
  //     for (int d = c+1; d <= num_points; ++d) {
  //       for (int e = 2; e <= num_points; ++e) {
  //         for (int f = e+1; f <= num_points; ++f) {
  //           for (int g = f+1; g <= num_points; ++g) {
  //             try {
  //               Snowpal snowpal(expr, num_points);
  //               snowpal.add_ball({a, b}).add_ball({c, d}).add_ball({e, f, g});
  //               analyze_snowpal(snowpal);
  //             } catch (const IllegalTreeCutException&) {}
  //           }
  //         }
  //       }
  //     }
  //   }
  // }

  // const int a = 1;
  // for (int b = 2; b <= num_points; ++b) {
  //   for (int c = 2; c <= num_points; ++c) {
  //     for (int d = c+1; d <= num_points; ++d) {
  //       for (int e = 2; e <= num_points; ++e) {
  //         for (int f = e+1; f <= num_points; ++f) {
  //           try {
  //             Snowpal snowpal(expr, num_points);
  //             snowpal.add_ball({a, b}).add_ball({c, d}).add_ball({e, f});
  //             analyze_snowpal(snowpal);
  //           } catch (const IllegalTreeCutException&) {}
  //         }
  //       }
  //     }
  //   }
  // }

  // const int a = 1;
  // for (int b = 3; b <= num_points; ++b) {
  //   for (int c = 2; c <= num_points; ++c) {
  //     for (int d = c+1; d <= num_points; ++d) {
  //       for (int e = 2; e <= num_points; ++e) {
  //         for (int f = e+1; f <= num_points; ++f) {
  //           for (int g = 2; g <= num_points; ++g) {
  //             for (int h = g+1; h <= num_points; ++h) {
  //               try {
  //                 Snowpal snowpal(expr, num_points);
  //                 snowpal.add_ball({a, b}).add_ball({c, d}).add_ball({e, f}).add_ball({g, h});
  //                 analyze_snowpal(snowpal);
  //               } catch (const IllegalTreeCutException&) {}
  //             }
  //           }
  //         }
  //       }
  //     }
  //   }
  // }

  for (int num_groups = 1; num_groups <= num_points / 2; ++num_groups) {  // group = 2 or more elements
    for (const auto& seq : all_sequences(num_groups + 1, num_points - 1)) {
      // Group 0 means "no group". Point 1 is always in group 1. Because symmetry.
      std::vector<std::vector<int>> groups(num_groups);
      groups[1 - 1].push_back(1);
      for (int i = 0; i < seq.size(); ++i) {
        const int point = i + 2;  // numeration starts from 1; skip point 1 (see above)
        const int group = seq[i];
        if (group > 0) {
          groups.at(group - 1).push_back(point);
        }
      }
      if (absl::c_any_of(groups, [](const auto& gr) { return gr.size() < 2; })) {
        continue;
      }
      try {
        Snowpal snowpal(expr, num_points);
        for (const auto& gr : groups) {
          snowpal.add_ball(gr);
        }
        analyze_snowpal(snowpal);
      } catch (const IllegalTreeCutException&) {}
    }
  }

  std::cout << stats;



/*
  constexpr char kInvalidInput[] = "Invalid input: ";
  bool short_form_ratios = true;
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
    reset_snowpal();
    std::cout << expr;
    while (true) {
      std::cout << "\n> ";
      std::string input;
      std::getline(std::cin, input);
      trim(input);
      if (input.empty()) {
        continue;
      } else if (input == "q" || input == "quit") {
        return 0;
      } else if (input == "sf" || input == "short_forms") {
        short_form_ratios = !short_form_ratios;
        if (short_form_ratios) {
          std::cout << "Short form cross-ratios: enabled\n";
        } else {
          std::cout << "Short form cross-ratios: disabled\n";
        }
        to_ostream(std::cout, *snowpal, short_form_ratios);
        continue;
      } else if (input == "r" || input == "reset") {
        break;
      } else if (input == "b" || input == "back") {
        if (balls.empty()) {
          std::cout << kInvalidInput << "nothing to remove\n";
          continue;
        }
        balls.pop_back();
        reset_snowpal();
        to_ostream(std::cout, *snowpal, short_form_ratios);
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
        to_ostream(std::cout, *snowpal, short_form_ratios);
      } catch (const IllegalTreeCutException& e) {
        std::cout << kInvalidInput << e.what() << "\n";
        reset_snowpal();
      }
    }
  }
*/


  // Snowpal snowpal(expr, 7);
  // snowpal.add_ball({1,2,3}).add_ball({1,3});  // => {1,x} + {x,1} == 0  // superseeded by the one below
  // snowpal.add_ball({1,3,7});  // => {x,y} == {1/x,1/y}
  // snowpal.add_ball({1,2,3}).add_ball({6,7});  // => {x,y} + {y,x} == 0
  // snowpal.add_ball({1,3,5}).add_ball({2,4});  // => {1,x} + {1,x/(x-1)} == 0
  // snowpal.add_ball({1,3,5}).add_ball({2,7});  // => {1,x} == 0
  // snowpal.add_ball({1,3,5});  // could be explored more
  // snowpal.add_ball({1,3}).add_ball({2,7});  // => {x,y} + {1/(1-y),1-1/x)} == 0
  // snowpal.add_ball({1,3}).add_ball({2,5});  // => {x,y} + {x/(x-1),y} == 0   ???


  // {1,3,6}{2,5,8}{4,7}  -- one var, first arg changes
  // {1,2,5}{3,6,9}
  // {1,3,5}{2,6,7}       -- two vars
  // {1,3,6}{2,4,9}{5,7}  -- close to (<1>, <1>, var)
  // {1,3,4,8}            -- {x, y, z} + {1/x, 1/y, 1/z} == 0
  // {1,2,4,6}            -- {x, y, z} + {1/z, 1/y, 1/x} == 0  (note: this is the one above + shuffle)
  // {1,2,4,7}{3,6}       -- two vars

  // Below are equation modulo <1>
  // {1,3}{5,8}{2,4,9}    -- {x, y, z} + {1-x, y/(y-1), 1-z} == 0

  // BAD: uses dependent variables
  // {1,4,6,8}{3,9}       -- {x, y, z} + {x, y, 1-z} == {with <= 2 args}
  // {1,3,5,7}{2,8}       -- {x, y, z} + {x/(x-1), y, z} == {with <= 2 args}
}
#endif
