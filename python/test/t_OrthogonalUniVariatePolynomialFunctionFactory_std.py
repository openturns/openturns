#! /usr/bin/env python

from __future__ import print_function
import openturns as ot


polynomialFactory = ot.LegendreFactory()
factory = ot.OrthogonalUniVariatePolynomialFunctionFactory(polynomialFactory)
print(factory)
x = 0.4
for i in range(10):
    function = factory.build(i)
    print('order=', i, function,
          'X=', ot.Point([x]),
          'f(X)=', ot.Point([function(x)]),
          'df(X)=', ot.Point([function.gradient(x)]),
          'd2f(X)=', ot.Point([function.hessian(x)]))
