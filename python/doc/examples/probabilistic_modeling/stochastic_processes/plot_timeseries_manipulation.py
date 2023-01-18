"""
Manipulate a time series
========================
"""
# %%
# The objective here is to create and manipulate a time series.
# A time series is a particular field where the mesh :math:`\mathcal{M}` 1-d and regular, eg a time grid :math:`(t_0, \dots, t_{N-1})`.
#
# It is possible to draw a time series, using interpolation between the values: see the use case on the Field.
#
# A time series can be obtained as a realization of a multivariate stochastic process
# :math:`X: \Omega \times [0,T] \rightarrow \mathbb{R}^d` of dimension :math:`d` where :math:`[0,T]`
# is discretized according to the regular grid :math:`(t_0, \dots, t_{N-1})`.
# The  values :math:`(\underline{x}_0, \dots, \underline{x}_{N-1})` of the  time series are defined by:
#
# .. math::
#    \forall i \in [0, N-1],\quad \underline{x}_i= X(\omega)(t_i)
#
#
# A time series is stored in the **TimeSeries** object that stores the regular time grid and the associated values.

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

ot.Log.Show(ot.Log.NONE)

# %%
# Create the RegularGrid
tMin = 0.0
timeStep = 0.1
N = 100
myTimeGrid = ot.RegularGrid(tMin, timeStep, N)

# %%
# Case 1: Create a time series from a time grid and values
# Care! The number of steps of the time grid must correspond to the size of the values
myValues = ot.Normal(3).getSample(myTimeGrid.getVertices().getSize())
myTimeSeries = ot.TimeSeries(myTimeGrid, myValues)
myTimeSeries

# %%
# Case 2: Get a time series from a Process
myProcess = ot.WhiteNoise(ot.Normal(3), myTimeGrid)
myTimeSeries2 = myProcess.getRealization()
myTimeSeries2

# %%
# Get the number of values of the time series
myTimeSeries.getSize()

# %%
# Get the dimension of the values observed at each time
myTimeSeries.getMesh().getDimension()

# %%
# Get the value Xi at index i
i = 37
print("Xi = ", myTimeSeries.getValueAtIndex(i))

# %%
# Get the time series at index i : Xi
i = 37
print("Xi = ", myTimeSeries[i])

# %%
# Get a the marginal value at index i of the time series
i = 37
# get the time stamp:
print("ti = ", myTimeSeries.getTimeGrid().getValue(i))
# get the first component of the corresponding value :
print("Xi1 = ", myTimeSeries[i, 0])

# %%
# Get all the values (X1, .., Xn) of the time series
myTimeSeries.getValues()

# %%
# Compute the temporal Mean
# It corresponds to the mean of the values of the time series
myTimeSeries.getInputMean()

# %%
# Draw the marginal i of the time series using linear interpolation
graph = myTimeSeries.drawMarginal(0)
view = viewer.View(graph)

# %%
# with no interpolation
graph = myTimeSeries.drawMarginal(0, False)
view = viewer.View(graph)
plt.show()
