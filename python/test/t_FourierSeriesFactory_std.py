#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

factory = ot.FourierSeriesFactory()
print(factory)
x = 0.4
for i in range(10):
    function = factory.build(i)
    print('order=', i, function,
      'X=', ot.NumericalPoint([x]),
      'f(X)=', ot.NumericalPoint([function(x)]),
      'df(X)=', ot.NumericalPoint([function.gradient(x)]),
      'd2f(X)=', ot.NumericalPoint([function.hessian(x)]))
