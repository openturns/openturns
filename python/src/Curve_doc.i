%feature("docstring") OT::Curve
"Curve.

Available constructors:
    Curve(*data, legend*)

    Curve(*dataX, dataY, legend*)

    Curve(*data, color, lineStyle, lineWidth, legend*)

Parameters
----------
data : 2-d sequence of float
    Points from which the curve is built.
dataX, dataY : two 2-d sequences of float of dimension 1, or two sequences of float
    Points from which the curve is built.
legend : str
    Legend of the Curve. By default, empty.
color : str
    Color of the curve. If not specified, by default it is the first color of the default palette.
lineStyle : str
    Style of the curve. If not specified, by default it is 'solid'.
lineWidth : float
    Width of the curve. If not specified, by default it is 1.

Examples
--------
>>> import openturns as ot
>>> from math import pi, sin
>>> # Generate the data for the curve to be drawn
>>> data = [[-3 * pi + 6 * pi * i / 100, sin(-3 * pi + 6 * pi * i / 100)] for i in range(100)]
>>> # Create an empty graph
>>> myGraph = ot.Graph('A curve', 'x', 'sin(x)', True, '')
>>> # Create the curve
>>> myCurve = ot.Curve(data, 'blue', 'solid', 2, '')
>>> myGraph.add(myCurve)"

// ---------------------------------------------------------------------

%feature("docstring") OT::Curve::getLabels
"Accessor to the labels drawn along the curve.

Returns
-------
labels : :class:`~openturns.Description`
    Labels drawn at evenly spaced points of the curve."

// ---------------------------------------------------------------------

%feature("docstring") OT::Curve::setLabels
"Accessor to the labels drawn along the curve.

Parameters
----------
labels : sequence of str
    Labels drawn at evenly spaced points of the curve."
