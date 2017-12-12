#!/bin/bash

if [ $EUID -ne 0 ]
then
    echo "need run this script as root"
    exit 1
fi

if ! [ -f ./test.sh ]
then
    echo "need run this in test dir"
    exit 1
fi

cd ..
if ! [ -d build ]
then
    mkdir build
fi
cd build

if [ "$1" == "-r" ]
then
    rm -rfv ./* && cmake ..
fi

cmake --build . --target install

cd ../test
if ! [ -d build ]
then
    mkdir build
fi
cd build

if [ "$1" == "-r" ]
then
    rm -rfv ./* && cmake ..
fi

cmake --build .

./gbtest 100
