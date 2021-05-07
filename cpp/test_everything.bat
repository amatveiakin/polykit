bazel test -c opt --config=msvc --platform_suffix=test_everything --cxxopt=/DRUN_LARGE_TESTS --cxxopt=/DRUN_HUGE_TESTS --test_output=errors :all_tests
