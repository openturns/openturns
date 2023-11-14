%feature("docstring") OT::Cloud
"Cloud.

Available constructors:
    Cloud(*data, legend=' '*)

    Cloud(*dataX, dataY, legend=' '*)

    Cloud(*data, color, pointStyle, legend=' '*)

    Cloud(*dataComplex, legend=' '*)

Parameters
----------
data : 2-d sequence of float
    Points from which the cloud is built.
dataX, dataY : two 2-d sequences of float of dimension 1, or two sequences of float
    Points from which the cloud is built.
legend : str
    Legend of the Cloud.
color : str
    Color of the points. If not specified, the default color is the first color in the default palette.
pointStyle : str
    Style of the points. If not specified, by default it is 'plus'.
dataComplex : :class:`~openturns.ComplexCollection`
    Collection of complex points.

Examples
--------
>>> import openturns as ot
>>> R = ot.CorrelationMatrix(2)
>>> R[1, 0] = -0.25
>>> distribution = ot.Normal([-1.5, 0.5], [4.0, 1.0], R)
>>> sample = distribution.getSample(100)
>>> # Create an empty graph
>>> myGraph = ot.Graph('Normal sample', 'x1', 'x2', True, '')
>>> # Create the cloud
>>> myCloud = ot.Cloud(sample, 'blue', 'fsquare', 'My Cloud')
>>> myGraph.add(myCloud)"
