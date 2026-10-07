#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

factory = ot.HaarWaveletFactory()
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

# Exercise every public method of the underlying HaarWavelet
functions = [factory.build(i) for i in range(4)]
for function in functions:
    assert function == function
    assert not (function != function)
    assert "HaarWavelet" in repr(function)
    assert "f:X" in str(function)
assert functions[0] != functions[1]
assert functions[1] != functions[2]
assert default != functions[0]

# Scaling function branch (order 0)
scaling = functions[0]
assert str(scaling) == "f:X -> {1.0 for 0.0<=X<1.0, 0.0 elsewhere}"
ott.assert_almost_equal(scaling(-0.5), 0.0)
ott.assert_almost_equal(scaling(0.0), 1.0)
ott.assert_almost_equal(scaling(0.5), 1.0)
ott.assert_almost_equal(scaling(1.5), 0.0)
ott.assert_almost_equal(scaling.gradient(0.5), 0.0)
ott.assert_almost_equal(scaling.hessian(0.5), 0.0)

# Wavelet branches: below support, left/right halves, above support
wavelet = factory.build(1)
ott.assert_almost_equal(wavelet(-0.1), 0.0)
ott.assert_almost_equal(wavelet(0.25), 1.0)
ott.assert_almost_equal(wavelet(0.75), -1.0)
ott.assert_almost_equal(wavelet(1.5), 0.0)
ott.assert_almost_equal(wavelet.gradient(0.25), 0.0)
ott.assert_almost_equal(wavelet.hessian(0.25), 0.0)

# Sample overload agrees with scalar calls, draw, exception branch
sample = ot.Sample([[-0.1], [0.25], [0.75], [1.5]])
for function in functions:
    values = function(sample)
    for i in range(sample.getSize()):
        ott.assert_almost_equal(values[i, 0], function(sample[i, 0]))
    function.draw(0.0, 1.0, 5)
    with ott.assert_raises(Exception):
        function(ot.Sample([[0.5, 0.5]]))
