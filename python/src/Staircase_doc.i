%feature("docstring") OT::Staircase
"Staircase.

Available constructors:
    Staircase(*data, legend=' '*)

    Staircase(*data, color, lineStyle, lineWidth, pattern, legend=' '*)

    Staircase(*data, color, lineStyle, pattern, legend=' '*)

Parameters
----------
data : 2-d sequence of float
    Points from which the Staircase is built, must be of dimension 2: the
    discontinuous points and their corresponding height.
legend : str
    Legend of the Staircase.
color : str
    Color of the curve. If not specified, the default color is the first color in the default palette.
lineStyle : str
    Style of the curve. If not specified, by default it is 'solid'.
lineWidth : float 
    Width of the curve. If not specified, by default it is 1.
pattern : str
    Pattern which is 'S' or 's'. By default the pattern is equal to 's'. Going
    from :math:`(x_1, y_1)` to :math:`(x_2, y_2)` with :math:`x_1<x_2`,
    pattern='s' moves first horizontal then vertical, whereas pattern='S' moves
    the other way around.

Examples
--------
>>> import openturns as ot
>>> distribution = ot.Normal([0.5], [2.], ot.CorrelationMatrix(1))
>>> sample = distribution.getSample(200)
>>> # Construct empirical CDF for the sample
>>> data = ot.Sample(20, 2)
>>> for i in range(20):
...     c0 = 13.0 * i / 20.0 - 6.5
...     count = 0
...     for j in range(200):
...         if sample[j, 0] < c0:
...             count += 1
...     c1 = count / 200.0
...     data[i] = [c0, c1]
>>> # Create an empty graph
>>> myGraph = ot.Graph('A staircase', 'x1', 'x2', True, 'upper left')
>>> # Create the staircase
>>> myStaircase = ot.Staircase(data, 'blue', 'solid', 's', 'Normal CDF')
>>> myGraph.add(myStaircase)"
