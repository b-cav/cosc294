pushd ../utils/
make clean > /dev/null
make > /dev/null
popd

make clean > /dev/null
make > /dev/null
make run
