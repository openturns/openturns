#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

# First, build two functions from R^3->R
inVar = ["x1", "x2", "x3"]
functions = list()
functions.append(
    ot.SymbolicFunction(
        inVar, ["x1^3 * sin(x2 + 2.5 * x3) - (x1 + x2)^2 / (1.0 + x3^2)"]
    )
)
functions.append(
    ot.SymbolicFunction(inVar, ["exp(-x1 * x2 + x3) / cos(1.0 + x2 * x3 - x1)"])
)
# Second, build the weights
coefficients = ot.Sample(0, 3)
coefficients.add([1.5, 2.5, -0.5])
coefficients.add([-3.5, 0.5, -1.5])
# Third, build the function
myFunction = ot.DualLinearCombinationFunction(functions, coefficients)
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

print("myFunction (HTML)=")
print(myFunction._repr_html_())

# Extra coverage: default ctor, comparison, string converters (no prints to keep expout stable)

defaultFunc = ot.DualLinearCombinationFunction()
assert isinstance(repr(defaultFunc), str)
assert isinstance(str(defaultFunc), str)
assert myFunction == myFunction
assert not (myFunction != myFunction)
sameFunc = ot.DualLinearCombinationFunction(functions, coefficients)
assert myFunction == sameFunc
assert not (myFunction != sameFunc)
otherFunctions = [functions[1], functions[0]]
assert myFunction != ot.DualLinearCombinationFunction(otherFunctions, coefficients)
assert isinstance(repr(myFunction), str)
assert isinstance(str(myFunction), str)
ott.assert_almost_equal(sameFunc(inPoint), myFunction(inPoint), 1e-12, 1e-12)
