bazel test -c opt --config=msvc --platform_suffix=test_extra --cxxopt=/DRUN_LARGE_TESTS --test_output=errors :all_tests
