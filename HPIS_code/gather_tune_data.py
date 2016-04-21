import os
import numpy
import sys

# First go through the directory

def isfloat(x):
	try:
		float(x)
		return True
	except ValueError:
		return False
num_epoch = 40
if (len(sys.argv) == 2):
	num_epoch = int(sys.argv[1])
tbl = []
subd = [x[0] for x in os.walk("param_tune_tmp")]
for idx in range(1, len(subd)):
	param_list = [float(s) for s in subd[idx].replace('/', '_').split('_') if isfloat(s)]
	data = numpy.loadtxt(subd[idx] + "/lossvalue.tbl")
	#print subd[idx].replace('/', '_').split('_')
	#print param_list
	#print data[39,1]
	tbl.append((data[num_epoch - 1,1], param_list[0], param_list[1]))
tbl = sorted(tbl)
for t in tbl:
	print t[1], t[2], t[0]
