%feature("docstring") OT::PocketFFT
R"RAW(PocketFFT implementation of the Fast Fourier Transform.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This is a wrapper on the `PocketFFT <https://github.com/hayguen/pocketfft>`_
header-only library and implements the :class:`~openturns.FFT` interface, so
it can be given to any accessor expecting a :class:`~openturns.FFT` algorithm.

The transform is complex to complex: a :class:`~openturns.Point` argument is
converted to a complex sequence with a zero imaginary part, as in
:class:`~openturns.KissFFT`.

Compared to :class:`~openturns.KissFFT`, the sizes which are not factorable
by the small radices 2, 3, 4, 5, 7, 8, 11 are not necessarily computed with
the direct discrete transform in :math:`O(N^2)` of the vendored kissfft
back-end, the case of a prime size :math:`N`: PocketFFT handles them with
Bluestein's chirp-z algorithm in :math:`O(N \log N)` when the size is at
least 50, its largest prime factor exceeds the square root of the size and
its estimated cost is lower than the one of the factorization, which is the
case of large prime sizes. Smaller sizes, such as the prime size 13, are
handled by the factorization.
The :math:`1 / N` normalization of the inverse transform is folded into the
transform factor.

The multidimensional transform applies the one-dimensional transform along
each axis of the shape given by ``dimensions``, over a flat column-major
(Fortran) storage where the first axis varies fastest, in the same order as
:class:`~openturns.KissFFT`. It is parallelized with the OpenTURNS ``TBB``
threads, the internal threading of the PocketFFT header being disabled.

See Also
--------
openturns.FFT
openturns.KissFFT

Notes
-----
- An empty ``dimensions`` is the identity for a collection holding exactly
  one value, the product of an empty shape being 1; with any other size, an
  exception is raised.
- A zero entry in ``dimensions``, or a data size different from the product
  of the entries, raises an exception.
- When the Bluestein algorithm is selected for a size, the convolution pads
  the transform to the smallest integer greater than or equal to
  :math:`2N - 1` whose only prime factors are 2, 3, 5, 7 and 11, so a prime
  size uses convolution buffers of roughly twice the transform length.

Examples
--------
The transform of a real sequence is a complex sequence:

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> fft = otexp.PocketFFT()
>>> print(fft)
class=PocketFFT
>>> collection = ot.ComplexCollection([1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0])
>>> result = fft.transform(collection)
>>> print(result)
[(4,0),...]

The inverse transform recovers the original sequence up to the round-off
errors:

>>> restored = fft.inverseTransform(result)
>>> [round(c.real, 6) + 0 for c in restored]  # + 0 turns -0.0 into 0.0
[1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0]

A prime transform size, which the vendored kissfft back-end computes with a
direct transform:

>>> data = ot.Point([float(i + 1) for i in range(13)])
>>> spectrum = fft.transform(data, [13])
>>> print(spectrum.getSize())
13
>>> restored = fft.inverseTransform(spectrum, [13])
>>> [round(c.real, 6) + 0 for c in restored]
[1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0]

The multidimensional transform over a flat column-major storage:

>>> data = ot.Point([float(i + 1) for i in range(6)])
>>> result = fft.transform(data, [2, 3])
>>> restored = fft.inverseTransform(result, [2, 3])
>>> [round(c.real, 6) + 0 for c in restored]
[1.0, 2.0, 3.0, 4.0, 5.0, 6.0]
)RAW"
