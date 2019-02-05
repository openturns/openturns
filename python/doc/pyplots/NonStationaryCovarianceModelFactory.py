import openturns as ot
from math import exp
from matplotlib import pyplot as plt
from openturns.viewer import View


# Create the covariance function at (s,t)

def C(s, t):
    return exp(-4.0 * abs(s - t) / (1 + (s * s + t * t)))


def covModRef(X):
    return [C(X[0], X[1])]


myFuncCovarianceRef = ot.PythonFunction(2, 1, covModRef)
myFuncCovarianceRef.setDescription(["s", "t", "C"])

t0 = -4.0
tmax = 4.0

# Draw the isocontours of the discretized covariance function
myGraphRef = myFuncCovarianceRef.draw([t0, t0], [tmax, tmax])
alld = myGraphRef.getDrawables()
levels = ot.Point(alld.getSize())
for i in range(alld.getSize()):
    d = alld[i]
    d.setLineStyle("twodash")
    d.setLineWidth(2)
    myGraphRef.setDrawable(d, i)
    levels[i] = d.getLevels()[0]


# Create the time grid
# for iN in range(2, 11):
#  t_00 = time()
N = 2**5
dt = (tmax - t0) / N
myMesh = ot.RegularGrid(t0, dt, N)

# Keep only time stamps in the time-grid
tmax = myMesh.getEnd()

# Create the collection of HermitianMatrix
covariance = ot.CovarianceMatrix(N)
for k in range(N):
    s = myMesh.getValue(k)
    for l in range(k + 1):
        t = myMesh.getValue(l)
        covariance[k, l] = C(s, t)

# Create the covariance model
myCovarianceModel = ot.UserDefinedCovarianceModel(
    myMesh, covariance)

# Create the non stationary Gaussian process with
# that covariance model
myProcess = ot.GaussianProcess(myCovarianceModel, myMesh)

# Create a  sample of fields
size = 10**4
myFieldSample = myProcess.getSample(size)

# Build a covariance model factory
myFactory = ot.NonStationaryCovarianceModelFactory()

# Estimation on a the ProcessSample
myEstimatedModel = myFactory.build(myFieldSample)

# Define the python function associated to myCovarianceModel


def covMod(X):
    return [myEstimatedModel(X[0], X[1])[0, 0]]


myFuncCovariance = ot.PythonFunction(2, 1, covMod)


cov_graph = ot.Graph(myGraphRef)
alld = myFuncCovariance.draw([t0, t0], [tmax, tmax]).getDrawables()
palette = ot.Drawable.BuildDefaultPalette(alld.getSize())
for i in range(alld.getSize()):
    d = alld[i]
    d.setLegend("")
    d.setLevels([levels[i]])
    d.setColor(palette[i])
    d.setDrawLabels(False)
    cov_graph.add(d)
cov_graph.setTitle('Non stationary covariance model estimation')

fig = plt.figure(figsize=(10, 4))
cov_axis = fig.add_subplot(111)
View(cov_graph, figure=fig, axes=[cov_axis],
     add_legend=False, square_axes=True)
