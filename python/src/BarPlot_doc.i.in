%feature("docstring") OT::BarPlot
"Bar plot.

Available constructors:
    BarPlot(*data, origin, legend=' '*)

    BarPlot(*data, origin, color, fillStyle, lineStyle, legend=' '*)

    BarPlot(*data, origin, color, fillStyle, lineStyle, lineWidth, legend=' '*)

Parameters
----------
data : 2-d sequence of float
    Data from which the BarPlot is built, must be of dimension 2: the
    discontinuous points and their corresponding height.
origin : float
    Scalar defining where the BarPlot begins.
legend : str
    Legend of the BarPlot.
color : str
    Color of the curve. If not specified, the default color is the first color in the default palette.
fillStyle : str
    Fill style of the surfaces. If not specified, by default it is 'solid'.
lineStyle : str
    Style of the curve. If not specified, by default it is 'solid'.
lineWidth : float
    Width of the curve. If not specified, by default it is 1.

Examples
--------
>>> import openturns as ot
>>> graph = ot.Graph('BarPlot', 'X', 'Y', True, 'topright')
>>> origin = 0.0
>>> data = [[1.0, 2.0], [3.0, 4.0], [2.0, 3.0]]
>>> barPlot = ot.BarPlot(data, origin, 'orange', 'shaded', 'dashed', 'label')
>>> graph.add(barPlot)"
