#!/bin/bash

while getopts m:r: flag
do
    case "${flag}" in
        m) make=${OPTARG};;
        r) run=${OPTARG};;
    esac
done
# echo "Make: $make";
# echo "Run: $run";

if [ "$make" != "" ]; then
    cd CUDA/src; make clean; make
    cd ../../OpenMP/src; make clean; make
    cd ../../OpenCL/src; make clean; make
    cd ../../pthread/src; make clean; make
    cd ../..
fi

if [ "$run" != "" ]; then
    cd pthread/src; ./target 2 1920.jpg
fi
