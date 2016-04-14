#!/bin/bash

g++ SGDTrain.cpp -o SGDTrain -O2 -std=c++11

mkdir Plot1/optimal
mkdir Plot1/proportional
mkdir Plot1/greedy

mkdir Plot2/optimal
mkdir Plot2/proportional
mkdir Plot2/greedy

mkdir Plot3/optimal
mkdir Plot3/proportional
mkdir Plot3/greedy

mkdir Plot4/optimal
mkdir Plot4/proportional
mkdir Plot4/greedy

mkdir Plot5/optimal
mkdir Plot5/proportional
mkdir Plot5/greedy

arr=(0 0.001 0.90)
#IFS=$' ' read -r -a arr < Plot5/optimal.Rlog
./SGDTrain 40000 S -n 6 -i Plot5/ R1 R2 R3 R4 R5 R6 -f 2 3 4 5 6 7 -r 1000 1200 1500 1800 2000 2500 -o Plot5/optimal -e 100 -a ${arr[1]} -d ${arr[2]} -m 3 -p 11 7 17 20 11 14 -v > Plot5/optimal/test.Tlog 2>&1 &

#IFS=$' ' read -r -a arr < Plot5/proportional.Rlog
./SGDTrain 40000 S -n 6 -i Plot5/ R1 R2 R3 R4 R5 R6 -f 2 3 4 5 6 7 -r 1000 1200 1500 1800 2000 2500 -o Plot5/proportional -e 100 -a ${arr[1]} -d ${arr[2]} -m 3 -p 8 11 12 14 15 20 -v > Plot5/proportional/test.Tlog 2>&1 &

#IFS=$' ' read -r -a arr < Plot5/greedy.Rlog
./SGDTrain 40000 S -n 6 -i Plot5/ R1 R2 R3 R4 R5 R6 -f 2 3 4 5 6 7 -r 1000 1200 1500 1800 2000 2500 -o Plot5/greedy -e 100 -a ${arr[1]} -d ${arr[2]} -m 3 -p 11 14 14 13 14 14 -v > Plot5/greedy/test.Tlog 2>&1 &

#IFS=$' ' read -r -a arr < Plot4/optimal.Rlog
./SGDTrain 40000 S -n 4 -i Plot4/ R1 R2 R3 R4 -f 2 3 4 5 -r 2000 2500 3000 3500 -o Plot4/optimal -e 100 -a ${arr[1]} -d ${arr[2]} -m 3 -p 22 28 17 13 -v > Plot4/optimal/test.Tlog 2>&1 &

#IFS=$' ' read -r -a arr < Plot4/proportional.Rlog
./SGDTrain 40000 S -n 4 -i Plot4/ R1 R2 R3 R4 -f 2 3 4 5 -r 2000 2500 3000 3500 -o Plot4/proportional -e 100 -a ${arr[1]} -d ${arr[2]} -m 3 -p 15 19 21 25 -v > Plot4/proportional/test.Tlog 2>&1 &

#IFS=$' ' read -r -a arr < Plot4/greedy.Rlog
./SGDTrain 40000 S -n 4 -i Plot4/ R1 R2 R3 R4 -f 2 3 4 5 -r 2000 2500 3000 3500 -o Plot4/greedy -e 100 -a ${arr[1]} -d ${arr[2]} -m 3 -p 20 18 20 18 -v > Plot4/greedy/test.Tlog 2>&1 &

#IFS=$' ' read -r -a arr < Plot3/optimal.Rlog
./SGDTrain 40000 S -n 2 -i Plot3/ R1 R2 -f 2 3 -r 3500 4000 -o Plot3/optimal -e 100 -a ${arr[1]} -d ${arr[2]} -m 3 -p 20 60 -v > Plot3/optimal/test.Tlog 2>&1 &

#IFS=$' ' read -r -a arr < Plot3/proportional.Rlog
./SGDTrain 40000 S -n 2 -i Plot3/ R1 R2 -f 2 3 -r 3500 4000 -o Plot3/proportional -e 100 -a ${arr[1]} -d ${arr[2]} -m 3 -p 38 42 -v > Plot3/proportional/test.Tlog 2>&1 &

#IFS=$' ' read -r -a arr < Plot3/greedy.Rlog
./SGDTrain 40000 S -n 2 -i Plot3/ R1 R2 -f 2 3 -r 3500 4000 -o Plot3/greedy -e 100 -a ${arr[1]} -d ${arr[2]} -m 3 -p 39 41 -v > Plot3/greedy/test.Tlog 2>&1 &

#IFS=$' ' read -r -a arr < Plot2/optimal.Rlog
./SGDTrain 100000 S -n 2 -i Plot2/ R1 R2 -f 2 3 -r 6000 10000 -o Plot2/optimal -e 100 -a ${arr[1]} -d ${arr[2]} -m 3 -p 33 31 -v > Plot2/optimal/test.Tlog 2>&1 &

#IFS=$' ' read -r -a arr < Plot2/proportional.Rlog
./SGDTrain 100000 S -n 2 -i Plot2/ R1 R2 -f 2 3 -r 6000 10000 -o Plot2/proportional -e 100 -a ${arr[1]} -d ${arr[2]} -m 3 -p 24 40 -v > Plot2/proportional/test.Tlog 2>&1 &

#IFS=$' ' read -r -a arr < Plot2/greedy.Rlog
./SGDTrain 100000 S -n 2 -i Plot2/ R1 R2 -f 2 3 -r 6000 10000 -o Plot2/greedy -e 100 -a ${arr[1]} -d ${arr[2]} -m 3 -p 32 32 -v > Plot2/greedy/test.Tlog 2>&1 &

#IFS=$' ' read -r -a arr < Plot1/optimal.Rlog
./SGDTrain 40000 S -n 2 -i Plot1/ R1 R2 -f 2 3 -r 3000 4000 -o Plot1/optimal -e 100 -a ${arr[1]} -d ${arr[2]} -m 3 -p 17 47 -v > Plot1/optimal/test.Tlog 2>&1 &

#IFS=$' ' read -r -a arr < Plot1/proportional.Rlog
./SGDTrain 40000 S -n 2 -i Plot1/ R1 R2 -f 2 3 -r 3000 4000 -o Plot1/proportional -e 100 -a ${arr[1]} -d ${arr[2]} -m 3 -p 28 36 -v > Plot1/proportional/test.Tlog 2>&1 &

#IFS=$' ' read -r -a arr < Plot1/greedy.Rlog
./SGDTrain 40000 S -n 2 -i Plot1/ R1 R2 -f 2 3 -r 3000 4000 -o Plot1/greedy -e 100 -a ${arr[1]} -d ${arr[2]} -m 3 -p 32 32 -v > Plot1/greedy/test.Tlog 2>&1 &
