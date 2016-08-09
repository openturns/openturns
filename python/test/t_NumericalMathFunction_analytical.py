#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import math as m

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

    f = ot.NumericalMathFunction(
        'x', '2.0*' + func + '(x)', 'y')
    print('f=', f)
    print('f(', x[0], ')=%.4e' % f(x)[0])
    try:
        df = f.gradient(x)[0, 0]
    except:
        pass
    else:
        f.setGradient(ot.CenteredFiniteDifferenceGradient(ot.ResourceMap.GetAsNumericalScalar(
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
        f.setHessian(ot.CenteredFiniteDifferenceHessian(ot.ResourceMap.GetAsNumericalScalar(
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

nmf = ot.NumericalMathFunction(['x0', 'x1'], ['y0', 'y1'], ['x0+x1', 'x0-x1'])
marginal0 = nmf.getMarginal(0)
marginal1 = nmf.getMarginal(1)
print('marginal 0=', marginal0)
print('marginal 1=', marginal1)

# test sample as input of a function
formula = ['sin(xi1) + 7. * (sin(xi2)) ^ 2 + 0.1 * xi3^4 * sin(xi1)']
model = ot.NumericalMathFunction(['xi1', 'xi2', 'xi3'], ['y'], formula)

# Create an input distribution to calculate reference values
distribution = ot.ComposedDistribution([ot.Uniform(-m.pi, m.pi)] * 3)
inSample = distribution.getSample(100)
resultSample = model(inSample)
refResultValues = [m.sin(inSample[i][0]) + 7.0 * (m.sin(inSample[i][1])) **
                   2 + 0.1 * (inSample[i][2])**4 * m.sin(inSample[i][0]) for i in range(100)]

print('First reference value : %.4e' % refResultValues[0])
print('First result calculated : %.4e' % resultSample[0][0])

model = ot.NumericalMathFunction(['x'], ['y1', 'y2'], ['_pi', '_e'])
print('Constants:', model([0]))
