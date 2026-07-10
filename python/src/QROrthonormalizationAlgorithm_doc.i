%feature("docstring") OT::QROrthonormalizationAlgorithm
"QR-based orthonormalization algorithm.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Notes
-----
This class computes an orthonormal basis from a finite set of functions using the QR
factorization of the weighted design matrix:

.. math::
    M_{ij} = \\sqrt{w_i}\\, f_j(x_i)

where :math:`(x_i, w_i)` are quadrature nodes and weights for the measure.

The orthonormal functions are given by the columns of :math:`Q R^{-1}` where
:math:`M = Q R`.

The following :class:`~openturns.ResourceMap` keys can be set:

- *QROrthonormalizationAlgorithm-DefaultDiscretization*: number of 1D nodes in the
  Gauss product experiment (default=128)
- *QROrthonormalizationAlgorithm-Epsilon*: threshold for zeroing coefficients
  (default=1.0e-11)

Parameters
----------
functions : sequence of :class:`~openturns.Function`
    Initial set of functions.
distribution : :class:`~openturns.Distribution`
    Measure with respect to which to orthonormalize.
experiment : :class:`~openturns.WeightedExperiment`, optional
    Weighted experiment for numerical integration.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> dim = 2
>>> distribution = ot.Normal(dim)
>>> refBasis = ot.OrthogonalProductPolynomialFactory([ot.HermiteFactory()] * dim)
>>> basis = [refBasis.build(i) for i in range(6)]
>>> algo = otexp.QROrthonormalizationAlgorithm(basis, distribution)
>>> algo.run()
>>> phi0 = algo.getOrthonormalFunctions()[0]
"

%feature("docstring") OT::QROrthonormalizationAlgorithm::run
"Compute the orthonormal basis."
