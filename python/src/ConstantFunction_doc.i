%feature("docstring") OT::ConstantFunction
"Constant function.

Parameters
----------
inputDimension : int, optional
    Input dimension, default is 1
constant : sequence of float, optional
    Constant value, default is [0]

Examples
--------
>>> import openturns as ot
>>> f = ot.ConstantFunction(2, [1.0, 2.0, 3.0])
>>> print(f([5.0, 6.0]))
[1,2,3]"
