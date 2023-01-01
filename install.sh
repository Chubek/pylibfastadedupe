#!/bin/bash

cc="g++"
ggd=""
gfl=""
python_headers="/usr/include/python3.10"
while getopts d?c:l:gf?ph: flag
do
    case "${flag}" in
        c) cc=${OPTARG};;
        l) ll=${OPTARG};;
        d) ggd="1";; 
        gf) gfl="sudo gfilt";;
        ph) python_headers=${OPTARG};;
    esac
done

echo "Compiler is $cc"


if [ "$ggd" = "1" ]
then
    echo "Running with debug flag."
fi

swig -c++ -python fastadedupe.i
$gflt make $init COMPILER=$cc DEBUG=$ggd PYTHON_HEADERS=$python_headers

