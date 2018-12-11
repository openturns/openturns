import openturns as ot
from math import exp
from matplotlib import pyplot as plt
from openturns.viewer import View

# Create the time grid
# In the context of the spectral estimate or Fourier transform use,
# we use data blocs with size of form 2^p
tMin = 0.
timeStep = 0.1
size = pow(2, 12)
myTimeGrid = ot.RegularGrid(tMin, timeStep, size)

# We fix the parameter of the Cauchy model
amplitude = [5]
scale = [3]
model = ot.CauchyModel(scale, amplitude)
gp = ot.SpectralGaussianProcess(model, myTimeGrid)

# Get a time series or a sample of time series
# myTimeSeries = gp.getRealization()
mySample = gp.getSample(1000)

mySegmentNumber = 10
myOverlapSize = 0.3

# Build a spectral model factory
myFactory = ot.WelchFactory(ot.Hanning(), mySegmentNumber, myOverlapSize)

# Estimation on a TimeSeries or on a ProcessSample
# myEstimatedModel_TS = myFactory.build(myTimeSeries)
myEstimatedModel_PS = myFactory.buildAsUserDefinedSpectralModel(mySample)

# Change the filtering window
myFactory.setFilteringWindows(ot.Hamming())

# Get the FFT algorithm
myFFT = myFactory.getFFTAlgorithm()

# Get the frequencyGrid
frequencyGrid = myEstimatedModel_PS.getFrequencyGrid()

# With the model, we want to compare values
# We compare values computed with theoritical values
plotSample = ot.Sample(frequencyGrid.getN(), 3)

# Loop of comparison ==> data are saved in plotSample
for k in range(frequencyGrid.getN()):
    freq = frequencyGrid.getStart() + k * frequencyGrid.getStep()
    plotSample[k, 0] = freq
    plotSample[k, 1] = abs(myEstimatedModel_PS(freq)[0, 0])
    plotSample[k, 2] = abs(model(freq)[0, 0])


# Graph section
# We build 2 curves
# each one is function of frequency values
ind = ot.Indices(2)
ind.fill()

# Some cosmetics : labels, legend position, ...
graph = ot.Graph("Spectral model estimation", "Frequency",
                 "Spectral density function", True, "topright", 1.0, ot.GraphImplementation.LOGY)

# The first curve is the estimate density as function of frequency
curve1 = ot.Curve(plotSample.getMarginal(ind))
curve1.setColor('blue')
curve1.setLegend('estimate model')

# The second curve is the theoritical density as function of frequency
ind[1] = 2
curve2 = ot.Curve(plotSample.getMarginal(ind))
curve2.setColor('red')
curve2.setLegend('Cauchy model')

graph.add(curve1)
graph.add(curve2)

fig = plt.figure(figsize=(10, 4))
graph_axis = fig.add_subplot(111)
view = View(graph, figure=fig, axes=[graph_axis], add_legend=False)
