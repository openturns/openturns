%feature("docstring") OT::BasisFactory
"Basis factory base class.

Parameters
----------
orthogUniVarPolFactory : :class:`~openturns.OrthogonalUniVariatePolynomialFactory`
    Factory that builds particular univariate polynomial (e.g. Hermite,
    Legendre, Laguerre, ...).

See also
--------
ConstantBasisFactory, LinearBasisFactory, QuadraticBasisFactory

Notes
-----
BasisFactory is the interface of the OrthogonalUniVariatePolynomialFactory
implementation. It represents the factory that allows the construction of any
univariate orthonormal polynomial with any degree."

// ---------------------------------------------------------------------

%feature("docstring") OT::BasisFactory::build
"Build the basis.

Returns
-------
basis : :class:`~openturns.Basis`."
