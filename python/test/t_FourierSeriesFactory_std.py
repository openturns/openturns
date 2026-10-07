#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

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

# Alternate ctors: default UniVariateFunction
default = ot.UniVariateFunction()
assert default == default
assert not (default != default)

# Exercise every public method of the underlying FourierSeries
functions = [factory.build(i) for i in range(4)]
for function in functions:
    assert function == function
    assert not (function != function)
    assert "FourierSeries" in repr(function)
    assert "f:X" in str(function)
assert functions[0] != functions[1]
assert functions[1] != functions[2]
assert functions[2] != functions[3]
assert default != functions[0]

# k == 0 branch: constant function with zero gradient and hessian
assert str(functions[0]) == "f:X -> 1"
ott.assert_almost_equal(functions[0](x), 1.0)
ott.assert_almost_equal(functions[0].gradient(x), 0.0)
ott.assert_almost_equal(functions[0].hessian(x), 0.0)

# k == 1 and k >= 2 branches, cosine and sine variants
assert "sin" in str(functions[1])
assert "cos" in str(functions[2])
assert "2 * X" in str(functions[3])

# Sample overload agrees with scalar calls, draw, exception branch
sample = ot.Sample([[0.1], [0.4], [0.9]])
for function in functions:
    values = function(sample)
    for i in range(sample.getSize()):
        ott.assert_almost_equal(values[i, 0], function(sample[i, 0]))
    function.draw(-3.14159, 3.14159, 5)
    with ott.assert_raises(Exception):
        function(ot.Sample([[0.5, 0.5]]))
