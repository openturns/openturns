%feature("docstring") OT::DatabaseFunction
R"RAW(Pre-evaluated function.

Available constructor:
    DatabaseFunction(*inputSample, outputSample*)

This class builds a function which is constant on the Voronoï
cells of the input sample.

Parameters
----------
inputSample : 2-d sequence of float
    Values of the inputs.
outputSample : 2-d sequence of float
    Values of the outputs.

Notes
-----
Let :math:`(\vect{x}_1, \dots, \vect{x}_n)` be the input sample and
:math:`(\vect{y}_1, \dots, \vect{y}_n)` be the output sample.
The data-based function built is such that for any :math:`1 \leq i \leq n`, we have
:math:`f(\vect{x}_i) = \vect{y}_i` and for any other point math:`\vect{x}`, we have
:math:`f(\vect{x}) = \vect{y}_{i(\vect{x})}`
where :math:`i(\vect{x})` is the index such that :math:`\vect{x}_{i(\vect{x})}` is the point of the
input sample the closest to math:`\vect{x}`. In other words, the function is constant on the Voronoï
cells of the input sample.

Examples
--------
>>> import openturns as ot
>>> inputSample = [[1.0, 1.0], [2.0, 2.0]]
>>> outputSample = [[4.0], [5.0]]
>>> database = ot.DatabaseFunction(inputSample, outputSample)
>>> x = [1.8] * database.getInputDimension()
>>> print(database(x))
[5])RAW"
