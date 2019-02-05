from __future__ import print_function
import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View
import math as m

oldPrecision = ot.PlatformInfo.GetNumericalPrecision()
ot.PlatformInfo.SetNumericalPrecision(16)
a = -m.pi
b = m.pi
f = ot.SymbolicFunction(['x', 'y'], ['1+cos(x)*sin(y)'])
l = [ot.SymbolicFunction(['x'], [' 2+cos(x)'])]
u = [ot.SymbolicFunction(['x'], ['-2-cos(x)'])]

# Draw the graph of the integrand and the bounds:

g = ot.Graph('IteratedQuadrature example', 'x', 'y', True, 'topright')
g.add(f.draw([a, a], [b, b]))
curve = l[0].draw(a, b).getDrawable(0)
curve.setLineWidth(2)
curve.setColor('red')
g.add(curve)
curve = u[0].draw(a, b).getDrawable(0)
curve.setLineWidth(2)
curve.setColor('red')
g.add(curve)

# Evaluate the integral with high precision:

Iref = ot.IteratedQuadrature(ot.GaussKronrod(100000, 1e-13,
                                             ot.GaussKronrodRule(ot.GaussKronrodRule.G11K23))).integrate(f, a, b, l, u)

# Evaluate the integral with the default GaussKronrod algorithm:

f = ot.MemoizeFunction(f)
I1 = ot.IteratedQuadrature(ot.GaussKronrod()).integrate(f, a, b, l, u)
sample1 = f.getInputHistory()
print('I1=', I1, '#evals=', sample1.getSize(),
      'err=', abs(100.0 * (1.0 - I1[0] / Iref[0])), '%')
cloud = ot.Cloud(sample1)
cloud.setPointStyle('fcircle')
cloud.setColor('green')
g.add(cloud)
f.clearHistory()

# Evaluate the integral with the default IteratedQuadrature algorithm:

I2 = ot.IteratedQuadrature().integrate(f, a, b, l, u)
sample2 = f.getInputHistory()
# print('I2=', I2, '#evals=', sample2.getSize(), \
#        'err=', abs(100.0*(1.0-I2[0]/Iref[0])), '%')
cloud = ot.Cloud(sample2)
cloud.setPointStyle('fcircle')
cloud.setColor('gold')
g.add(cloud)

fig = plt.figure(figsize=(4, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(g, figure=fig, axes=[axis], add_legend=False)
ot.PlatformInfo.SetNumericalPrecision(oldPrecision)
