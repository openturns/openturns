#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# Product
# First, build a function from R^3->R
inVar = ["x0", "x1", "x2"]
formula = ["x0^2 + 2 * x1 * x2 + 3 * x2"]
f1 = ot.SymbolicFunction(inVar, formula)
# Second, build a function from R^3->R^2
formula = ["x2 - x0 + x1"]
formula.append("x0 + x1 * x0 + x2")
f2 = ot.SymbolicFunction(inVar, formula)
# Third, build the product function
myFunction = f1 * f2
inPoint = ot.Point([1.2, 2.3, 3.4])
print("myFunction=", myFunction)
value = myFunction(inPoint)
print("Value at  %s =\n%s" % (inPoint, value))
gradient = myFunction.gradient(inPoint)
print("Gradient at  %s =\n%s" % (inPoint, gradient))
hessian = myFunction.hessian(inPoint)
print("Hessian at  %s =\n%s" % (inPoint, hessian))

# Sum/difference
# First, build two functions from R^3->R^2
inVar = ["x0", "x1", "x2"]
formula = ["x0 + 2 * x1 * x2 + 3 * x2", "x2 - x0 + x1 * x0"]
f1 = ot.SymbolicFunction(inVar, formula)
formula = ["x0 + x1 + x2", "-2 * x0 + 3 * x2 * x1 - x1"]
f2 = ot.SymbolicFunction(inVar, formula)
# Second, build the function
mySum = f1 + f2
print("mySum=", mySum)
value = mySum(inPoint)
print(f"Value at  {inPoint} =\n{value}")
gradient = mySum.gradient(inPoint)
print(f"Gradient at  {inPoint} =\n{gradient}")
hessian = mySum.hessian(inPoint)
print(f"Hessian at  {inPoint} =\n{hessian}")
myDiff = f1 - f2
print("myDiff=", myDiff)
value = myDiff(inPoint)
print(f"Value at  {inPoint} =\n{value}")
gradient = myDiff.gradient(inPoint)
print(f"Gradient at  {inPoint} =\n{gradient}")
hessian = myDiff.hessian(inPoint)
print(f"Hessian at  {inPoint} =\n{hessian}")
