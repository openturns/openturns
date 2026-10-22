%feature("docstring") OT::KissFFT
"KissFFT implementation.

This is a wrapper on the KissFFT library and implements the :class:`~openturns.FFT` interface.

See also
--------
FFT

Examples
--------
Compute the discrete Fourier transform of a real sequence:

>>> import openturns as ot
>>> fft = ot.KissFFT()
>>> collection = ot.ComplexCollection([1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0])
>>> result = fft.transform(collection)
>>> print(result)
[(4,0),...]

The inverse transform recovers the original sequence up to the round-off errors:

>>> restored = fft.inverseTransform(result) # doctest: +SKIP
"
