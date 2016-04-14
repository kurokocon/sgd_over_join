import numpy
import matplotlib.pyplot as plt

for i in range(1,6):
	plotDir = "Plot" + str(i)
	plt.clf()
	methodList = ("optimal", "proportional", "greedy")
	colorList = ("r", "g", "b")
	for j in range(3):
		data = numpy.loadtxt(plotDir + "/" + methodList[j] + "/lossvalue.tbl")
		x = data[:,0]
		y = data[:,1]
		plt.xlabel('Epoch')
		plt.ylabel('Loss value')
		plt.plot(x, y, colorList[j])
	plt.savefig(plotDir + ".svg")
