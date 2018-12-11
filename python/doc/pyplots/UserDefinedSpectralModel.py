import openturns as ot
from math import exp
from matplotlib import pyplot as plt
from openturns.viewer import View


# Create the frequency grid
fmin = 0.1
df = 0.2
N = int((10.0 - fmin) / df)
myFrequencyGrid = ot.RegularGrid(fmin, df, N)

# Spectral function


def s(f):
    if(f <= 5.0):
        return 1.0
    else:
        x = f - 5.0
        return exp(-2.0 * x * x)


# Create the collection of HermitianMatrix
myCollection = ot.HermitianMatrixCollection()
for k in range(N):
    frequency = myFrequencyGrid.getValue(k)
    matrix = ot.HermitianMatrix(1)
    matrix[0, 0] = s(frequency)
    myCollection.add(matrix)


# Create the spectral model
mySpectralModel = ot.UserDefinedSpectralModel(myFrequencyGrid, myCollection)

# Graph of the spectral function
x = ot.Sample(N, 2)
for k in range(N):
    frequency = myFrequencyGrid.getValue(k)
    x[k, 0] = frequency
    value = mySpectralModel(frequency)
    x[k, 1] = value[0, 0].real

# Create the curve of the spectral function
myCurve = ot.Curve(x, 'UserSpectral')

# Create the graph
myGraph = ot.Graph('User Defined Spectral Model', 'Frequency',
                   'Spectral density value', True)
myGraph.add(myCurve)
myGraph.setLegendPosition('topright')

fig = plt.figure(figsize=(10, 4))
cov_axis = fig.add_subplot(111)
view = View(myGraph, figure=fig, axes=[cov_axis], add_legend=False)
