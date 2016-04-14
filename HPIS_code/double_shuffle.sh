i=0
while [ $((2**i)) -lt 80000 ]; do
	echo $((40000 / (2**i)))
	IFS=$' ' read -r -a arr < $i/param.Rlog
	./SGDTrain_old 40000 S -n 1 -i ./ R -f 2 -r 4000 -o $i/ -e 100 -a ${arr[1]} -d ${arr[2]} -m 3 -p $((2**i)) -v > $i/test.Tlog 2>&1 &
	i=$((i+1))
done
