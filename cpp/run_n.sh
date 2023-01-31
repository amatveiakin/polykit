bazel build -c opt --config=clang :workspace
for i in $(seq 1 $1)
do
    ../bazel-bin/cpp/workspace $i &
done
wait
