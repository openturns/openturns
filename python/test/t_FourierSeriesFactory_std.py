#! /usr/bin/env python

import openturns as ot

factory = ot.FourierSeriesFactory()
print(factory)
x = 0.4
for i in range(10):
    function = factory.build(i)
    print(
        "order=",
        i,
        function,
        "X=",
        ot.Point([x]),
        "f(X)=",
        ot.Point([function(x)]),
        "df(X)=",
        ot.Point([function.gradient(x)]),
        "d2f(X)=",
        ot.Point([function.hessian(x)]),
    )
