#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

# 1. Basic 1D linear function
func1 = ot.SymbolicFunction(['x1', 'x2'], ['3*x1 - 2*x2 +5'])
lin1 = func1.isLinear()
lin10 = func1.isLinearlyDependent(0)
lin11 = func1.isLinearlyDependent(1)
assert(lin1)
assert(lin10)
assert(lin11)

# 2. Basic 2D linear function
func2 = ot.SymbolicFunction(['x1', 'x2'], ['3*x1 - 2*x2 +5', '-x1 + x2'])
lin2 = func2.isLinear()
lin20 = func2.isLinearlyDependent(0)
lin21 = func2.isLinearlyDependent(1)
assert(lin2)
assert(lin20)
assert(lin21)

# 3. 2D non linear function
func3 = ot.SymbolicFunction(['x1', 'x2'], ['3*x1 - 2*x2 +5', '-x1^2 + x2'])
lin3 = func3.isLinear()
lin30 = func3.isLinearlyDependent(0)
lin31 = func3.isLinearlyDependent(1)
assert(not lin3)
assert(not lin30)
assert(lin31)


# 4. 2D non linear function
func4 = ot.SymbolicFunction(['x1', 'x2'], ['3*x1*x2'])
lin4 = func4.isLinear()
lin40 = func4.isLinearlyDependent(0)
lin41 = func4.isLinearlyDependent(1)
assert(not lin4)
assert(lin40)
assert(lin41)

# 5. Aggregated function
func5 = ot.AggregatedFunction([func2, func4])
lin5 = func5.isLinear()
lin50 = func5.isLinearlyDependent(0)
lin51 = func5.isLinearlyDependent(1)
assert(not lin5)
assert(lin50)
assert(lin51)

# 6. Python function


def pyFunc6(x):
    x1, x2 = x
    result = [3*x1 - 2*x2 + 5, -x1 + x2, 3*x1*x2]
    return result


def funLinearity():
    return False


def varLinearity(index):
    return True


func6 = ot.PythonFunction(
    2, 3, pyFunc6, functionLinearity=funLinearity, variablesLinearity=varLinearity)
lin6 = func6.isLinear()
lin60 = func6.isLinearlyDependent(0)
lin61 = func6.isLinearlyDependent(1)
assert(not lin6)
assert(lin60)
assert(lin61)
