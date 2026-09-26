%feature("docstring") OT::KissFFT
"Kiss FFT.

See also
--------
FFT

Notes
-----
The KissFFT class inherits from the :class:`~openturns.FFT` class. The methods
are the same as the FFT class (there is no additional method). This class
interacts with the kissfft implemented and return results as OpenTURNS objects
(:class:`~openturns.ComplexCollection`).

Examples
--------
Compute the discrete Fourier transform of a real sequence:

>>> import openturns as ot
>>> fft = ot.KissFFT()
>>> collection = ot.ComplexCollection([1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0])
>>> result = fft.transform(collection)
>>> print(result)
[(4,0),(1,-2.41421),(0,0),(1,-0.414214),(0,0),(1,0.414214),(0,0),(1,2.41421)]

The inverse transform recovers the original sequence up to the round-off errors:

>>> restored = fft.inverseTransform(result) # doctest: +SKIP
"
