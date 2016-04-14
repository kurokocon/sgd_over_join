i=0
while [ $((2**i)) -le 40000 ]; do
	mkdir $i
	Rscript param_opt.R T $((2**i)) > $i/param.Rlog 2>&1 &
	i=$((i+1))
done
