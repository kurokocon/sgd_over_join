#!/bin/bash

g++ gen_data_multi.cpp -Wall -Wextra -o gen_data_multi -O2 -std=c++11
for i in $(seq 1 5); do
mkdir Plot$i
done
./gen_data_multi 40000 5 1.0 2 Plot1/S Plot1/T 3000 4000 10 10 Plot1/R1 Plot1/R2 0 &
./gen_data_multi 100000 5 1.0 2 Plot2/S Plot2/T 6000 10000 10 10 Plot2/R1 Plot2/R2 0 &
./gen_data_multi 40000 5 1.0 2 Plot3/S Plot3/T 3500 4000 10 10 Plot3/R1 Plot3/R2 0 &
./gen_data_multi 40000 5 1.0 4 Plot4/S Plot4/T 2000 2500 3000 3500 10 10 10 10 Plot4/R1 Plot4/R2 Plot4/R3 Plot4/R4 0 &
./gen_data_multi 40000 5 1.0 6 Plot5/S Plot5/T 1000 1200 1500 1800 2000 2500 10 10 10 10 10 10 Plot5/R1 Plot5/R2 Plot5/R3 Plot5/R4 Plot5/R5 Plot5/R6 0 &
