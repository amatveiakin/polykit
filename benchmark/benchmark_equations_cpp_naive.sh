bazel run -c opt --config=clang --cxxopt=-DUNROLL_SHUFFLE=0 --cxxopt=-DUNROLL_SHUFFLE_MULTI=0 --cxxopt=-DDISABLE_PACKING=1 :benchmark_equations_cpp
