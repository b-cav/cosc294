#!/bin/bash

pushd ../utils/ > /dev/null
make clean > /dev/null
make > /dev/null
popd > /dev/null

make clean > /dev/null
make > /dev/null

echo -e "COMPILE\n-----------------"
./compile < test.scm > test.bc

echo -e "\nRAW BYTE CODE\n-----------------"
hexdump -C test.bc

echo -e "\nINTERPRET\n-----------------"
./interpret < test.bc

