#! /usr/bin/env python

import openturns as ot

# First, build two functions from R^3->R^2
functions = list()
functions.append(
    ot.SymbolicFunction(
        ["x1", "x2", "x3"],
        [
            "x1^3 * sin(x2 + 2.5 * x3) - (x1 + x2)^2 / (1.0 + x3^2)",
            "x1^1 * sin(x3 + 2.5 * x1) - (x2 + x3)^2 / (1.0 + x1^2)",
        ],
    )
)
functions.append(
    ot.SymbolicFunction(
        ["x1", "x2", "x3"],
        [
            "exp(-x1 * x2 + x3) / cos(1.0 + x2 * x3 - x1)",
            "exp(-x2 * x3 + x1) / cos(1.0 + x3 * x1 - x2)",
        ],
    )
)
# Second, build the function
myFunction = ot.AggregatedFunction(functions)
inPoint = ot.Point([1.2, 2.3, 3.4])
print("myFunction=", myFunction)
print("Value at ", inPoint, "=", myFunction(inPoint))
print("Gradient at ", inPoint, "=", myFunction.gradient(inPoint))
ot.PlatformInfo.SetNumericalPrecision(5)
print("Hessian at ", inPoint, "=", myFunction.hessian(inPoint))
for i in range(myFunction.getOutputDimension()):
    print("Marginal ", i, "=", myFunction.getMarginal(i))
print("Marginal (0,1)=", myFunction.getMarginal([0, 1]))
print("Marginal (0,2)=", myFunction.getMarginal([0, 2]))
print("Marginal (1,2)=", myFunction.getMarginal([1, 2]))
