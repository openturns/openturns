%define OT_FourierSeriesFactory_doc
R"RAW(Fourier series function factory.

Generates the Fourier basis defined as follows:

.. math::

    \begin{array}{lcl}
      \psi_0(x)      & = & 1 \\
      \psi_{2k+1}(x) & = & \sqrt{2}\sin(kx) \\
      \psi_{2k+2}(x) & = & \sqrt{2}\cos(kx)
    \end{array}

This basis is orthogonal with respect to the uniform distribution
with minimum bound :math:`-\pi` and maximum bound :math:`\pi`.
Moreover, the basis functions have unit norm.

See also
--------
OrthogonalUniVariatePolynomialFunctionFactory

Examples
--------
The next example creates a tensor-product basis of Fourier functions.

>>> import openturns as ot
>>> inputDimension = 3
>>> familyColl = [ot.FourierSeriesFactory()] * inputDimension
>>> enumerateFunction = ot.LinearEnumerateFunction(inputDimension)
>>> basis = ot.OrthogonalProductFunctionFactory(familyColl, enumerateFunction)
)RAW"
%enddef
%feature("docstring") OT::FourierSeriesFactory
OT_FourierSeriesFactory_doc

