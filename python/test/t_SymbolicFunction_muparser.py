#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import math as m

ot.ResourceMap.Set("SymbolicParser-Backend", "MuParser")

parsers = ot.SymbolicFunction.GetValidParsers()
assert "MuParser" in parsers, 'MuParser not found'

elementaryFunctions = ['sin', 'cos', 'tan']
elementaryFunctions.append('asin')
elementaryFunctions.append('acos')
elementaryFunctions.append('atan')
elementaryFunctions.append('sinh')
elementaryFunctions.append('cosh')
elementaryFunctions.append('tanh')
elementaryFunctions.append('asinh')
elementaryFunctions.append('acosh')
elementaryFunctions.append('atanh')
elementaryFunctions.append('log2')
elementaryFunctions.append('log10')
elementaryFunctions.append('log')
elementaryFunctions.append('ln')
elementaryFunctions.append('lngamma')
elementaryFunctions.append('gamma')
elementaryFunctions.append('exp')
elementaryFunctions.append('erf')
elementaryFunctions.append('erfc')
elementaryFunctions.append('sqrt')
elementaryFunctions.append('cbrt')
elementaryFunctions.append('besselJ0')
elementaryFunctions.append('besselJ1')
elementaryFunctions.append('besselY0')
elementaryFunctions.append('besselY1')
elementaryFunctions.append('sign')
elementaryFunctions.append('rint')
elementaryFunctions.append('abs')
elementaryFunctions.append('min')
elementaryFunctions.append('max')
elementaryFunctions.append('sum')
elementaryFunctions.append('avg')
elementaryFunctions.append('floor')
elementaryFunctions.append('ceil')
elementaryFunctions.append('trunc')
elementaryFunctions.append('round')


# Check the creation of the elementary functions
for func in elementaryFunctions:
    x = [0.4]
    # acosh only defined for 1 <= x <= pi
    if func == 'acosh':
        x[0] = 1.4

    f = ot.SymbolicFunction(['x'], ['2.0*' + func + '(x)'])
    print('f=', f)
    print('f(', x[0], ')=%.4e' % f(x)[0])
    try:
        df = f.gradient(x)[0, 0]
    except:
        pass
    else:
        f.setGradient(ot.CenteredFiniteDifferenceGradient(ot.ResourceMap.GetAsScalar(
            'CenteredFiniteDifferenceGradient-DefaultEpsilon'), f.getEvaluation()))
        df2 = f.gradient(x)[0, 0]
        print('df(', x[0], ')=%.4e' % df, 'df (FD)=%.4e' % df2)
        if abs(df) > 1e-5:
            err_g = abs(df2 / df - 1.)
        else:
            err_g = abs(df - df2)
        if err_g > 1e-5:
            print('GRADIENT ERROR! check ' + func +
                  ' gradient, err=%.12g' % err_g)
    try:
        d2f = f.hessian(x)[0, 0, 0]
    except:
        pass
    else:
        f.setHessian(ot.CenteredFiniteDifferenceHessian(ot.ResourceMap.GetAsScalar(
            'CenteredFiniteDifferenceHessian-DefaultEpsilon'), f.getEvaluation()))
        d2f2 = f.hessian(x)[0, 0, 0]
        print('d2f(', x[0], ')=%.4e' % d2f, 'd2f (FD)=%.4e' % d2f2)
        if abs(d2f) > 1e-5:
            err_h = abs(d2f2 / d2f - 1.)
        else:
            err_h = abs(d2f - d2f2)
        if err_h > 1e-4:
            print('HESSIAN ERROR! check ' + func +
                  ' hessian, err=%.12g' % err_h)

nmf = ot.SymbolicFunction(['x0', 'x1'], ['x0+x1', 'x0-x1'])
marginal0 = nmf.getMarginal(0)
marginal1 = nmf.getMarginal(1)
print('marginal 0=', marginal0)
print('marginal 1=', marginal1)

# test sample as input of a function
formula = ['sin(xi1) + 7. * (sin(xi2)) ^ 2 + 0.1 * xi3^4 * sin(xi1)']
model = ot.SymbolicFunction(['xi1', 'xi2', 'xi3'], formula)

# Create an input distribution to calculate reference values
distribution = ot.ComposedDistribution([ot.Uniform(-m.pi, m.pi)] * 3)
inSample = distribution.getSample(100)
resultSample = model(inSample)
refResultValues = [m.sin(inSample[i][0]) + 7.0 * (m.sin(inSample[i][1])) **
                   2 + 0.1 * (inSample[i][2])**4 * m.sin(inSample[i][0]) for i in range(100)]

print('First reference value : %.4e' % refResultValues[0])
print('First result calculated : %.4e' % resultSample[0][0])

model = ot.SymbolicFunction(['x'], ['pi_', 'e_'])
print('Constants:', model([0]))

empty = model.getMarginal([])
x = [42.0]
y = empty(x)
print('empty eval', y)

constants = ot.SymbolicFunction.GetValidConstants()
assert len(constants) > 0, 'empty'
funcs = ot.SymbolicFunction.GetValidFunctions()
assert len(funcs) > 0, 'empty'
ops = ot.SymbolicFunction.GetValidOperators()
assert len(ops) > 0, 'empty'
print('OK')

# str ctor
model = ot.SymbolicFunction('x', '3*x')
assert model(x)[0] == 3.0 * x[0], 'str ctor eval'

# Check constants
f = ot.SymbolicFunction("x", "e_")
print(f, ", e_=", f([0.0]))
f = ot.SymbolicFunction("x", "pi_")
print(f, ", pi_=", f([0.0]))
# Check unary operators
f = ot.SymbolicFunction("x", "-x")
print(f, ", f([1])=", f([1.0]))
f = ot.SymbolicFunction("x", "(x=2.0)*x")
print(f, ", f([1])=", f([1.0]))
f = ot.SymbolicFunction("x", "!x")
print(f, ", f([0])=", f([0.0]))
print(f, ", f([1])=", f([1.0]))
print(f, ", f([1.5])=", f([1.5]))
print(f, ", f([-0.5])=", f([-0.5]))
# Check binary operators
f = ot.SymbolicFunction(["x", "y"], ["x && y"])
print(f, ", f([0, 0])=", f([0.0, 0.0]))
print(f, ", f([1, 0])=", f([1.0, 0.0]))
print(f, ", f([1, 1])=", f([1.0, 1.0]))
print(f, ", f([0, 1])=", f([0.0, 1.0]))
print(f, ", f([1.5, -0.5])=", f([1.5, -0.5]))
f = ot.SymbolicFunction(["x", "y"], ["x || y"])
print(f, ", f([0, 0])=", f([0.0, 0.0]))
print(f, ", f([1, 0])=", f([1.0, 0.0]))
print(f, ", f([1, 1])=", f([1.0, 1.0]))
print(f, ", f([0, 1])=", f([0.0, 1.0]))
print(f, ", f([1.5, -0.5])=", f([1.5, -0.5]))
f = ot.SymbolicFunction(["x", "y"], ["x ^^ y"])
print(f, ", f([0, 0])=", f([0.0, 0.0]))
print(f, ", f([1, 0])=", f([1.0, 0.0]))
print(f, ", f([1, 1])=", f([1.0, 1.0]))
print(f, ", f([0, 1])=", f([0.0, 1.0]))
print(f, ", f([1.5, -0.5])=", f([1.5, -0.5]))
f = ot.SymbolicFunction(["x", "y"], ["x <= y"])
print(f, ", f([1, 2])=", f([1.0, 2.0]))
print(f, ", f([1, 1])=", f([1.0, 1.0]))
print(f, ", f([2, 1])=", f([2.0, 1.0]))
f = ot.SymbolicFunction(["x", "y"], ["x >= y"])
print(f, ", f([1, 2])=", f([1.0, 2.0]))
print(f, ", f([1, 1])=", f([1.0, 1.0]))
print(f, ", f([2, 1])=", f([2.0, 1.0]))
f = ot.SymbolicFunction(["x", "y"], ["x != y"])
print(f, ", f([1, 2])=", f([1.0, 2.0]))
print(f, ", f([1, 1])=", f([1.0, 1.0]))
print(f, ", f([2, 1])=", f([2.0, 1.0]))
f = ot.SymbolicFunction(["x", "y"], ["x == y"])
print(f, ", f([1, 2])=", f([1.0, 2.0]))
print(f, ", f([1, 1])=", f([1.0, 1.0]))
print(f, ", f([2, 1])=", f([2.0, 1.0]))
f = ot.SymbolicFunction(["x", "y"], ["x > y"])
print(f, ", f([1, 2])=", f([1.0, 2.0]))
print(f, ", f([1, 1])=", f([1.0, 1.0]))
print(f, ", f([2, 1])=", f([2.0, 1.0]))
f = ot.SymbolicFunction(["x", "y"], ["x < y"])
print(f, ", f([1, 2])=", f([1.0, 2.0]))
print(f, ", f([1, 1])=", f([1.0, 1.0]))
print(f, ", f([2, 1])=", f([2.0, 1.0]))
f = ot.SymbolicFunction(["x", "y"], ["x + y"])
print(f, ", f([1, 2])=", f([1.0, 2.0]))
f = ot.SymbolicFunction(["x", "y"], ["x - y"])
print(f, ", f([1, 2])=", f([1.0, 2.0]))
f = ot.SymbolicFunction(["x", "y"], ["x * y"])
print(f, ", f([2, 3])=", f([2.0, 3.0]))
f = ot.SymbolicFunction(["x", "y"], ["x / y"])
print(f, ", f([2, 3])=", f([2.0, 3.0]))
f = ot.SymbolicFunction(["x", "y"], ["x ^ y"])
print(f, ", f([2, 3])=", f([2.0, 3.0]))
# Check for exceptional output
f = ot.SymbolicFunction("x", "sqrt(x)")
try:
    print(f, ", f([-1])=", f([-1.0]))
except:
    print(f, ", f([-1]) not defined")

ot.ResourceMap.SetAsBool("SymbolicParser-CheckResult", False)
f = ot.SymbolicFunction("x", "sqrt(x)")
print(f, ", f([-1]) is normal?", ot.SpecFunc.IsNormal(f([-1.0])[0]))
