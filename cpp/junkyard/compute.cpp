{
#if 0
  static constexpr int w = 6;
  DeltaExpr lido_a8;
  lido_a8 -= 3 * LidoSymmVec(w, {1,2,3,4,5,6,7,8});
  for (const auto seq : increasing_sequences(8, 6)) {
    const auto args = mapped(seq, [](int x) { return x + 1; });
    const int sign_proto = absl::c_accumulate(args, 0);
    const int sign = neg_one_pow(sign_proto);
    lido_a8 += sign * LidoSymmVec(w, args);
  }
  for (const auto seq : increasing_sequences(8, 4)) {
    const auto args = mapped(seq, [](int x) { return x + 1; });
    const int sign_proto = absl::c_accumulate(args, 0);
    const int sign = neg_one_pow(sign_proto);
    lido_a8 -= sign * LidoVec(w, args);
  }
  // auto expr = to_lyndon_basis(lido_a8);
  // print_sorted_by_num_distinct_variables(std::cout, expr);
#endif

#if 0
  static constexpr int w = 6;
  Profiler profiler;
  DeltaExpr lido_sum_a9;
  for (const auto seq : increasing_sequences(9, 8)) {
    const auto args = mapped(seq, [](int x) { return x + 1; });
    const int sign_proto = absl::c_accumulate(args, 0);
    const int sign = neg_one_pow(sign_proto);
    lido_sum_a9 += sign * LidoSymmVec(w, args);
  }
  for (const auto seq : increasing_sequences(9, 6)) {
    const auto args = mapped(seq, [](int x) { return x + 1; });
    const int sign_proto = absl::c_accumulate(args, 0);
    const int sign = neg_one_pow(sign_proto);
    lido_sum_a9 -= sign * LidoSymmVec(w, args);
  }
  for (const auto seq : increasing_sequences(9, 4)) {
    const auto args = mapped(seq, [](int x) { return x + 1; });
    const int sign_proto = absl::c_accumulate(args, 0);
    const int sign = neg_one_pow(sign_proto);
    lido_sum_a9 += sign * LidoVec(w, args);
  }
  profiler.finish("Expr");
  auto expr = to_lyndon_basis(project_on_x1(lido_sum_a9));
  profiler.finish("Lyndon");
  std::cout << "\n";
  // print_sorted_by_num_distinct_variables(std::cout, expr);
  std::cout << expr;
#endif

#if 1
  static constexpr int w = 1;
  static constexpr int total_args = 5;
  Profiler profiler;
  DeltaExpr lido_sum;
  for (int num_args = 4; num_args <= total_args; num_args += 2) {
    for (const auto seq : increasing_sequences(total_args, num_args)) {
      const auto args = mapped(seq, [](int x) { return x + 1; });
      const int sign_proto = absl::c_accumulate(args, 0) + num_args / 2;
      const int sign = neg_one_pow(sign_proto);
      lido_sum += sign * LidoSymmVec(w, args);
    }
  }
  profiler.finish("Expr");
  // auto expr = to_lyndon_basis(project_on_x1(lido_sum));
  auto expr = to_lyndon_basis(lido_sum);
  profiler.finish("Lyndon");
  std::cout << "\n";
  // print_sorted_by_num_distinct_variables(std::cout, expr);
  std::cout << expr;
#endif

#if 0
  auto expr = sum_looped_vec([](const auto& args) {
    return LidoSymm4(args);
  }, 7, {1,2,3,4,5,6});
  for (const auto seq : increasing_sequences(7, 4)) {
    const auto args = mapped(seq, [](int x) { return x + 1; });
    const int sign_proto = absl::c_accumulate(args, 0);
    const int sign = neg_one_pow(sign_proto);
    expr += sign * LidoVec(4, args);
  }
  // expr +=
  //   + Lido4(1,2,3,4)
  //   - Lido4(1,2,3,5)
  //   + Lido4(1,2,3,6)
  //   - Lido4(1,2,3,7)
  //   + Lido4(1,2,4,5)
  //   - Lido4(1,2,4,6)
  //   + Lido4(1,2,4,7)
  //   + Lido4(1,2,5,6)
  //   - Lido4(1,2,5,7)
  //   + Lido4(1,2,6,7)
  //   - Lido4(1,3,4,5)
  //   + Lido4(1,3,4,6)
  //   - Lido4(1,3,4,7)
  //   - Lido4(1,3,5,6)
  //   + Lido4(1,3,5,7)
  //   - Lido4(1,3,6,7)
  //   + Lido4(1,4,5,6)
  //   - Lido4(1,4,5,7)
  //   + Lido4(1,4,6,7)
  //   - Lido4(1,5,6,7)
  //   + Lido4(2,3,4,5)
  //   - Lido4(2,3,4,6)
  //   + Lido4(2,3,4,7)
  //   + Lido4(2,3,5,6)
  //   - Lido4(2,3,5,7)
  //   + Lido4(2,3,6,7)
  //   - Lido4(2,4,5,6)
  //   + Lido4(2,4,5,7)
  //   - Lido4(2,4,6,7)
  //   + Lido4(2,5,6,7)
  //   + Lido4(3,4,5,6)
  //   - Lido4(3,4,5,7)
  //   + Lido4(3,4,6,7)
  //   - Lido4(3,5,6,7)
  //   + Lido4(4,5,6,7)
  // ;
  // std::cout << to_lyndon_basis(expr);
  std::cout << terms_with_num_distinct_variables(to_lyndon_basis(expr), 4);
#endif

#if 0
  // static constexpr int w = 6;

  // auto expr_main = sum_looped_vec([](const std::vector<X>& args) {
  //   return LidoSymmVec(w, args);
  // }, 7, {1,2,3,4,5,6});
  // auto lido_a6_looped = [](const std::vector<int>& args_outer) {
  //   return sum_looped_vec([](const std::vector<X>& args) {
  //     return LidoVec(w, args);
  //   }, 7, args_outer);
  // };
  // auto expr_correction =
  //   + lido_a6_looped({1,2,3,4})
  //   - lido_a6_looped({1,2,3,5})
  //   + lido_a6_looped({1,2,3,6})
  //   - lido_a6_looped({1,2,4,6})
  //   + lido_a6_looped({1,2,5,6})
  // ;
  // auto expr = expr_main + expr_correction;
  // // auto expr = expr_main;
  // // auto lyndon = to_lyndon_basis(project_on_x1(expr));
  // // print_sorted_by_num_distinct_elements(std::cout, lyndon);
  // auto lyndon = to_lyndon_basis(expr);
  // std::cout << lyndon;

  static auto lido_a6_looped = [](const std::vector<int>& args_lido) {
    return sum_looped_vec([](const std::vector<X>& args_inner) {
      return LidoVec(w, args_inner);
    }, 6, args_lido, SumSign::alternating);
  };

  static auto lido_a8_looped = [](const std::vector<int>& args_lido) {
    return sum_looped_vec([](const std::vector<X>& args_inner) {
      return LidoVec(w, args_inner);
    }, 8, args_lido, SumSign::alternating);
  };

  auto lido_fixed = [](const std::vector<X>& args) {
    CHECK_EQ(args.size(), 6);
    return
      +2*LidoSymmVec(w, args)
      + delta_expr_substitute(
        +2*lido_a6_looped({1,2,3,4})
        -  lido_a6_looped({1,2,4,6})
        , args
      )
      // + delta_expr_substitute(
      //   sum_looped_vec([](const std::vector<X>& args_inner) {
      //     auto args_fixed = concat({Inf}, args_inner);
      //     return
      //       + LidoVec(w, choose_indices(args_fixed, {1,2,3,4}))
      //       - LidoVec(w, choose_indices(args_fixed, {1,2,3,5}))
      //       + LidoVec(w, choose_indices(args_fixed, {1,2,3,6}))
      //       - LidoVec(w, choose_indices(args_fixed, {1,2,4,6}))
      //       + LidoVec(w, choose_indices(args_fixed, {1,2,5,6}))
      //     ;
      //     }
      //     ,
      //     6, {1,2,3,4,5,6}, SumSign::alternating
      //   )
      //   , args
      // )
      // + delta_expr_substitute(
      //   + lido_a6_looped({1,2,3,4})
      //   - lido_a6_looped({1,2,3,5})
      //   + lido_a6_looped({1,2,3,6})
      //   - lido_a6_looped({1,2,4,6})
      //   + lido_a6_looped({1,2,5,6})
      //   , args
      // )
      ;
  };

  // auto lido_a6_final_base = []() {
  //   return
  //     +2*LidoVec(w, {1,2,3,4,5,6})
  //     -2*LidoVec(w, {1,2,3,4})
  //     -2*LidoVec(w, {3,4,5,6})
  //     -2*LidoVec(w, {5,6,1,2})
  //     +2*lido_a6_looped({1,2,3,4})
  //     -  lido_a6_looped({1,2,4,6})
  //   ;
  // };

  static auto lido_a6_final_base = []() {
    return
      +2*LidoVec(w, {1,2,3,4,5,6})
      -2*LidoVec(w, {2,3,4,5})
      -2*LidoVec(w, {4,5,6,1})
      -2*LidoVec(w, {6,1,2,3})
      -  lido_a6_looped({1,2,4,6})
    ;
  };

  static auto lido_a6_final = [](const std::vector<X>& args) {
    return delta_expr_substitute(lido_a6_final_base(), args);
  };

  // std::cout << "Base " << lido_a6_final_base() << "\n";
  // auto expr = sum_looped_vec(lido_a6_final, 7, {1,2,3,4,5,6});
  // // auto expr = lido_a6_final({1,2,3,4,5,6}) + lido_a6_final({2,3,4,5,6,1});
  // auto lyndon = to_lyndon_basis(expr);
  // std::cout << lyndon;
  // // std::cout << terms_with_num_distinct_variables(lyndon, 4);
  // // print_sorted_by_num_distinct_variables(std::cout, lyndon);



  // auto lido_a8_final_base =
  //   +4*LidoVec(w, {1,2,3,4,5,6,7,8})
  //   -4*LidoVec(w, {2,3,4,5,6,7})
  //   -4*LidoVec(w, {4,5,6,7,8,1})
  //   -4*LidoVec(w, {6,7,8,1,2,3})
  //   -4*LidoVec(w, {8,1,2,3,4,5})
  //   -2*lido_a8_looped({1,2,3,4,6,8})
  //   +  lido_a8_looped({1,2,3,5,6,8})
  // ;

  // auto lido_a8_final_base =
  //   + LidoVec(w, {1,2,3,4,5,6,7,8})
  //   - LidoVec(w, {2,3,4,5,6,7})
  //   - LidoVec(w, {4,5,6,7,8,1})
  //   - LidoVec(w, {6,7,8,1,2,3})
  //   - LidoVec(w, {8,1,2,3,4,5})
  // ;

  // auto lido_a8_final_base = lido_a8;

  auto lido_a8_final_base =
    +  lido_a8
    // +2*lido_a8_looped({1,2,3,4,5,6})
    // -  lido_a8_looped({1,2,3,4,6,8})
  ;

  auto lido_a8_final = [&](const std::vector<X>& args) {
    return delta_expr_substitute(lido_a8_final_base, args).annotate(
      fmt::function(
        fmt::sub_num("SuperLido", {w}),
        mapped(args, [](X x) { return to_string(x); })
      )
    );
  };

  std::cout << "Base " << lido_a8_final_base << "\n";
  Profiler profiler;
  auto expr = sum_looped_vec(lido_a8_final, 9, {1,2,3,4,5,6,7,8});
  profiler.finish("Expr");
  // auto expr = lido_a8_final({1,2,3,4,5,6,7,8}) + lido_a8_final({2,3,4,5,6,7,8,1});
  // auto lyndon = to_lyndon_basis(expr);
  // std::cout << lyndon;
  // std::cout << terms_with_num_distinct_variables(lyndon, 5);
  // print_sorted_by_num_distinct_variables(std::cout, lyndon);
  // std::cout << "\nsum of lido_a8_looped({1,2,3,4,6,8}) = " <<
  //     terms_with_min_distinct_elements(
  //       to_lyndon_basis(
  //         project_on_x1(
  //           sum_looped_vec(
  //             lido_a8_looped({1,2,3,4,6,8}),
  //             9, {1,2,3,4,5,6,7,8}
  //           )
  //         )
  //       ),
  //     5) << "\n";
  auto lyndon = to_lyndon_basis(project_on_x1(expr));
  profiler.finish("Lyndon");
  std::cout << "\n";
  std::cout << terms_with_min_distinct_elements(lyndon, 5);
  // print_sorted_by_num_distinct_elements(std::cout, lyndon);
#endif
}
