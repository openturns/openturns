#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

dist1 = ot.Normal(1.0, 0.5)
print('dist1:', dist1)
result = dist1 + 2.0
print('dist1+2:', result)
graph = result.drawPDF()

result = dist1 - 2.0
print('dist1-2:', result)
graph = result.drawPDF()

result = dist1 * 2.0
print('dist1*2:', result)
graph = result.drawPDF()

result = dist1 / 2.0
print('dist1/2:', result)
graph = result.drawPDF()

result = 2.0 / dist1
print('2/dist1:', result)

result = dist1.cos()
print('cos(dist1):', result)
graph = result.drawPDF()

result = dist1.sin()
print('sin(dist1):', result)
graph = result.drawPDF()

result = dist1.tan()
print('tan(dist1):', result)
# graph = result.drawPDF()


dist0 = ot.Uniform(-0.999, 0.999)
result = dist0.acos()
print('acos(dist0):', result)
graph = result.drawPDF()

result = dist0.asin()
print('asin(dist0):', result)
graph = result.drawPDF()

result = dist0.atan()
print('atan(dist0):', result)
graph = result.drawPDF()


result = dist1.cosh()
print('cosh(dist1):', result)
graph = result.drawPDF()

result = dist1.sinh()
print('sinh(dist1):', result)
graph = result.drawPDF()

result = dist1.tanh()
print('tanh(dist1):', result)
graph = result.drawPDF()


distG1 = ot.LogNormal(1.0, 1.0, 1.0)
result = distG1.acosh()
print('acosh(distG1):', result)
graph = result.drawPDF()

result = dist1.asinh()
print('asinh(dist1):', result)
graph = result.drawPDF()

result = dist0.atanh()
print('atanh(dist0):', result)
graph = result.drawPDF()


result = dist1.exp()
print('exp(dist1):', result)
graph = result.drawPDF()

result = distG1.log()
print('log(distG1):', result)
graph = result.drawPDF()

result = distG1.ln()
print('ln(distG1):', result)
graph = result.drawPDF()


result = dist1 ** 3
print('dist1^3:', result)
graph = result.drawPDF()

result = distG1 ** 2.5
print('dist1^2.5:', result)
graph = result.drawPDF()

result = distG1.inverse()
print('inverse(distG1):', result)
graph = result.drawPDF()

result = dist1.sqr()
print('sqr(dist1):', result)
graph = result.drawPDF()

result = distG1.sqrt()
print('sqrt(distG1):', result)
graph = result.drawPDF()

result = dist1.cbrt()
print('cbrt(dist1):', result)
graph = result.drawPDF()

result = dist1.abs()
print('abs(dist1):', result)
graph = result.drawPDF()


dist2 = ot.Normal(-2.0, 1.0)
result = dist1 + dist2
print('dist1+dist2:', result)
graph = result.drawPDF()

result = dist1 - dist2
print('dist1-dist2:', result)
graph = result.drawPDF()

result = dist1 * dist2
print('dist1*dist2:', result)
graph = result.drawPDF()

result = dist1 / dist2
print('dist1/dist2:', result)
# graph = result.drawPDF()


result = ot.LogNormal() * ot.LogNormal()
print('logn*logn:', result)
graph = result.drawPDF()

result = ot.LogUniform() * ot.LogUniform()
print('logu*logu:', result)
graph = result.drawPDF()

result = ot.LogUniform() * ot.LogNormal()
print('logu*logn:', result)
graph = result.drawPDF()

result = ot.LogNormal() * ot.LogUniform()
print('logn*logu:', result)
graph = result.drawPDF()

# For ticket #917
result = ot.WeibullMin() + ot.Exponential()
print('WeibullMin+Exponential:', result)
print('result.CDF(1.0)=%.6f' % result.computeCDF(1.0))
result = -1.0 * ot.WeibullMin() + ot.Exponential()
print('-WeibullMin+Exponential:', result)
print('result.CDF(1.0)=%.6f' % result.computeCDF(1.0))
result = ot.WeibullMin() - ot.Exponential()
print('WeibullMin-Exponential:', result)
print('result.CDF(1.0)=%.6f' % result.computeCDF(1.0))
result = -1.0 * ot.WeibullMin() - ot.Exponential()
print('-WeibullMin-Exponential:', result)
print('result.CDF(-1.0)=%.6f' % result.computeCDF(-1.0))

# 2-d
print(ot.Normal(2) + ot.Normal(2))
print(ot.Normal(2) + 3.0)
print(ot.Normal(2) - ot.Normal(2))
print(ot.Normal(2) - 3.0)

# unary minus
x = ot.Normal(7.0, 2.0)
print(-x)

# simplification of sum
x = -ot.Exponential() - ot.Exponential()
print(x)
