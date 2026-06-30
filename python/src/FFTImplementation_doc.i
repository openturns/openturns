%define OT_FFT_doc
"Base class for Fast Fourier Transform (FFT) and Inverse Fast Fourier Transform (IFFT).

Notes
-----
Perform FFT and IFFT with arbitrary array shape via the dimensions argument.
"
%enddef

%feature("docstring") OT::FFTImplementation
OT_FFT_doc
// ---------------------------------------------------------------------


%define OT_FFT_transform_doc
R"RAW(Perform Fast Fourier Transform (FFT).

Parameters
----------
collection : :class:`~openturns.ComplexCollection` or :class:`~openturns.ScalarCollection`, sequence of float
  Data to transform.
dimensions : :class:`~openturns.Indices`, optional
  Shape of the tensor for multidimensional FFT. If provided, the data
  is interpreted as an N-dimensional tensor stored in column-major order.
  If omitted, a 1D FFT is performed.

Returns
-------
collection : :class:`~openturns.ComplexCollection`
  The data in Fourier domain.

Notes
-----
The 1D Fast Fourier Transform writes as:

.. math::

    {\rm y_k} = \sum_{n=0}^{N-1} x_n exp(-2 i \pi \frac{kn}{N})

where :math:`x` denotes the data to be transformed, of size :math:`N`.

For multidimensional data, the FFT is computed by applying 1D FFT
sequentially along each dimension. The data is stored in column-major
(Fortran) order, where the first dimension varies fastest.

.. math::

    {\rm Y_{k_0,\ldots,k_{N-1}}} = \sum_{n_0=0}^{D_0-1}\cdots\sum_{n_{N-1}=0}^{D_{N-1}-1} X_{n_0,\ldots,n_{N-1}} \prod_{d=0}^{N-1} \exp\left(-2 i \pi \frac{k_d n_d}{D_d}\right)


Examples
--------
>>> import openturns as ot
>>> fft = ot.FFT()
>>> result = fft.transform(ot.Normal(8).getRealization())
>>> # 2D transform (2x3 matrix)
>>> data = ot.Point([1.0, 2.0, 3.0, 4.0, 5.0, 6.0])
>>> result = fft.transform(data, [2, 3])
)RAW"
%enddef

%feature("docstring") OT::FFTImplementation::transform
OT_FFT_transform_doc

// ---------------------------------------------------------------------

%define OT_FFT_inverseTransform_doc
R"RAW(Perform Inverse Fast Fourier Transform (IFFT).

Parameters
----------
collection : :class:`~openturns.ComplexCollection` or :class:`~openturns.ScalarCollection`, sequence of float
  Data to transform.
dimensions : :class:`~openturns.Indices`, optional
  Shape of the tensor for multidimensional IFFT. If provided, the data
  is interpreted as an N-dimensional tensor stored in column-major order.
  If omitted, a 1D IFFT is performed.

Returns
-------
collection : :class:`~openturns.ComplexCollection`
    The transformed data.

Notes
-----
The 1D Inverse Fast Fourier Transform writes as:

.. math::

    {\rm y_k} = \sum_{n=0}^{N-1} \frac{1}{N} x_n exp(2 i \pi \frac{kn}{N})

where :math:`x` denotes the data, of size :math:`N`, to be transformed.

For multidimensional data, the IFFT is computed by applying 1D IFFT
sequentially along each dimension, scaled by :math:`1/\prod D_d`.


Examples
--------
>>> import openturns as ot
>>> fft = ot.FFT()
>>> collection = ot.ComplexCollection([1+1j,2-0.3j,5-.3j,6+1j,9+8j,16+8j,0.3])
>>> result = fft.inverseTransform(collection)
>>> data = ot.Point([1.0, 2.0, 3.0, 4.0])
>>> result = fft.inverseTransform(data, [4])
)RAW"
%enddef

%feature("docstring") OT::FFTImplementation::inverseTransform
OT_FFT_inverseTransform_doc
