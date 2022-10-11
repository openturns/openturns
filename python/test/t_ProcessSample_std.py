#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# size of timeGrid
size = 6
dimension = 1
sample = ot.Sample(size, dimension)
for i in range(size):
    for j in range(dimension):
        sample[i, j] = i + j + 1

# TimeGrid
timeGrid = ot.RegularGrid(0.0, 1.0 / (size - 1), size)

# TimeSeries
timeSeries = ot.TimeSeries(timeGrid, sample)

# We create an empty ProcessSample with default constructor
psample0 = ot.ProcessSample()
psample0.setName("PSample0")
print("Default constructor")
print("psample0=", psample0)

# We create an empty ProcessSample with timeGrid, size and dimension
# arguments
psample1 = ot.ProcessSample(timeGrid, 4, dimension)
psample1.setName("PSample1")
print("Constructor based on size, dimension and timeGrid")
print("psample1=", psample1)

# change the first component using operator []
psample1[0] = timeSeries
print("changing psample1[0] with []")
print("psample1[0]=", (psample1[0]))

# We create a ProcessSample with size and timeSeries arguments
psample2 = ot.ProcessSample(3, timeSeries)
psample2.setName("PSample2")
print("Constructor based on size / timeSeries")
print("psample2=", psample2)

psample3 = ot.ProcessSample(8, timeSeries)
psample3.erase(2)
assert psample3.getSize() == 7, "wrong size"
psample3.erase(2, 5)
assert psample3.getSize() == 4, "wrong size"
psample3.clear()
assert psample3.getSize() == 0, "wrong size"

# TimeSeriesCollection
# collection = TimeSeriesCollection()
# collection.add(timeSeries)

# sample *= [0.1]
# newtimeSeries = TimeSeries(timeGrid, sample)
# collection.add(newtimeSeries)

# We create a ProcessSample with collection arguments arguments
# psample3 = ProcessSample(collection)

# psample3.setName("PSample3")
# print "Constructor based on collection "
# print "psample3=", psample3

# ts = psample3[1]
# print "last element of psample3=", (ts)
# print "psample3 mean=", psample3.computeMean()
# print "psample3 temporal mean=", psample3.computeTemporalMean()

# A process with 1D domain dimension and 2D output dimension
numberOfIntervals = 20
interval = ot.Interval(-1.0, 1.0)
mesh = ot.IntervalMesher([numberOfIntervals - 1]).build(interval)
outputDimension = 2
sampleSize = 250

processSample = ot.ProcessSample(mesh, sampleSize, outputDimension)
# processSample.setDescription(['Twater', 'Tair'])

R = ot.CorrelationMatrix(outputDimension)
R[0, 1] = 0.75
distribution = ot.Normal([15, 20], [5, 10], R)
for i in range(sampleSize):
    processSample[i] = distribution.getSample(mesh.getVerticesNumber())
mean = processSample.computeMean()
stddev = processSample.computeStandardDeviation()
sample_node4 = processSample.getSampleAtVertex(4)
assert len(sample_node4) == len(processSample), "wrong len"
print("mean=", mean.getValues())
print("sttdev=", stddev.getValues())
graph1 = processSample.draw()
graph2 = processSample.drawMarginalCorrelation(0, 1)
graph3 = processSample.drawCorrelation()
# center
processSample -= processSample.computeMean().getValues()
if 0:
    from openturns.viewer import View

    View(graph1).save("graph1.png")
    View(graph2).save("graph2.png")
    View(graph3).save("graph3.png")

# ctor from collection of Samples
coll = [
    ot.Normal(outputDimension).getSample(mesh.getVerticesNumber())
    for i in range(sampleSize)
]
processSample2 = ot.ProcessSample(mesh, coll)
assert len(processSample2) == sampleSize, "wrong size"

# More statistical methods
# processSample = ot.GaussianProcess(ot.MaternModel([10.0], [0.1], 1.5),  ot.RegularGrid(0.0, 0.1, 1000)).getSample(10000)
print("min=", processSample.getMin().getValues())
print("max=", processSample.getMax().getValues())
print("range=", processSample.computeRange().getValues())
print("variance=", processSample.computeVariance().getValues())
print("skewness=", processSample.computeSkewness().getValues())
print("kurtosis=", processSample.computeKurtosis().getValues())
print("centered moment (3)=", processSample.computeCentralMoment(3).getValues())
print("raw moment (3)=", processSample.computeRawMoment(3).getValues())
x = [0.2] * processSample.getDimension()
print("median=", processSample.computeMedian().getValues())
q = 0.3
print(
    "quantile at level",
    q,
    "=",
    processSample.computeQuantilePerComponent(q).getValues(),
)
print("empirical CDF at", x, "=", processSample.computeEmpiricalCDF(x).getValues())
print(
    "complementary empirical CDF at",
    x,
    "=",
    processSample.computeEmpiricalCDF(x, True).getValues(),
)

n = mesh.getVerticesNumber()
try:
    s = processSample.getSampleAtVertex(n)
    assert False, "should not go there"
except:
    pass
