#!/bin/bash

a=( 1.0 0.1 0.01 0.001 )
d=( 0.98 0.9 0.8 0.6 )

mkdir param_tune_tmp

for i in "${a[@]}"
do
	for j in "${d[@]}"
	do
		mkdir param_tune_tmp/"$i"_"$j"
		./SGDTrain_old 40000 S -n 1 -i ./ R -f 2 -r 4000 -o param_tune_tmp/"$i"_"$j"/ -e 100 -a $i -d $j -m 3 -p $1 -v > param_tune_tmp/"$i"_"$j"/test.Tlog 2>&1 &
	done
done
wait
python gather_tune_data.py $2 > tune_partition_"$1"_"$2".tbl 2>&1
