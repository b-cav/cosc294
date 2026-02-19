#!/bin/bash

if [[ !($# == 1 && $1 == "-nm") ]] ; then
    pushd ../utils/ > /dev/null
    make clean > /dev/null
    make > /dev/null
    popd > /dev/null

    make clean > /dev/null
    make > /dev/null
fi

echo -e "PARSE & COMPILE\n----------------------"
./compile < test.scm > test.bc

echo -e "\nRAW BYTE CODE\n----------------------"
hexdump -C test.bc

echo -e "\nINTERPRET\n----------------------"
./interpret < test.bc

