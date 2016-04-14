import numpy
import matplotlib.pyplot as plt

for endEpoch in [1, 5, 10, 40]:
	x=[]
	y=[]
	plt.clf()
	for i in range(0, 17):
		data = numpy.loadtxt(str(i) + "/lossvalue.tbl")
		x.append(i)
		y.append(data[endEpoch - 1,1])
	plt.xlabel('#Chunks in terms of power of two')
	plt.ylabel('Loss Value')
	plt.plot(x,y)
	plt.savefig(str(endEpoch) + "epoch.svg")
