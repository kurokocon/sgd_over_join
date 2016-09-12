import numpy
import matplotlib.pyplot as plt

def getNumber(name):
	ret = 0
	with open(name) as f:
		for line in f:
			if 'Time taken' in line:
				ret = float(line.split(':')[1])
	return ret

fr = 3
tr = 5
chunkfactor = 1
dS = 6
nR = 10000000
buffer = '1GB'
workmem = '1GB'
approach = "sonce"
listval = [1,5,10,15]
	
x1 = listval
y1 = []
x2 = listval
y2 = []
for tr in listval:
	y1.append(getNumber('{0}_fr_{1}_tr_{2}_chunkfactor_{3}_dS_{4}_nR_{5}_buffer_{6}_workmem_m_{7}.out'
	.format(fr,tr,chunkfactor,dS,nR,buffer,workmem,approach)))
	y2.append(getNumber('{0}_fr_{1}_tr_{2}_chunkfactor_{3}_dS_{4}_nR_{5}_buffer_{6}_workmem_hpis_{7}.out'
	.format(fr,tr,chunkfactor,dS,nR,buffer,workmem,approach)))

plt.xlabel('Tuple Ratio')
plt.ylabel('Time taken')
plt.plot(x1, y1, 'b', label='Materalize')
plt.plot(x2, y2, 'r', label='HPIS')
plt.legend()
plt.savefig('tr_{0}.svg'.format(approach))