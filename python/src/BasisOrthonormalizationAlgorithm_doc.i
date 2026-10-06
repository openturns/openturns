%feature("docstring") OT::BasisOrthonormalizationAlgorithm
R"RAW(Basis orthonormalization algorithm.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Notes
-----
This class computes an orthonormal basis from a finite set of functions
defined on the reference cube :math:`[-1, 1]^d`, with respect to a
multivariate absolutely continuous distribution. The physical-to-reference
mapping is handled internally. Two routes are used depending on the
integration rule:

- experiment-derived rules expose their nodes: the basis comes from the QR
  factorization of the weighted design matrix, which preserves the
  conditioning of the design;
- opaque rules only provide integrals: the basis comes from the Cholesky
  factorization :math:`G = R^T R` of the Gram matrix, whose condition
  number is the square of the design's, hence less robust on
  ill-conditioned bases.

A custom weighted experiment either targets the measure (nodes in the
support) or lives on the reference cube; any other range is rejected. A
custom :class:`~openturns.IntegrationAlgorithm` integrates against the
Lebesgue measure, the density being accounted for internally.

The following :class:`~openturns.ResourceMap` keys are used:

- ``BasisOrthonormalizationAlgorithm-DefaultDiscretization`` (``UnsignedInteger``, default: ``128``): number of 1D nodes in the Gauss product experiment.
- ``BasisOrthonormalizationAlgorithm-Epsilon`` (``Scalar``, default: ``1.0e-11``): threshold for zeroing coefficients.

Parameters
----------
functions : sequence of :class:`~openturns.Function`
    Initial set of functions.
distribution : :class:`~openturns.Distribution`
    Measure with respect to which to orthonormalize.
experiment : :class:`~openturns.WeightedExperiment`, optional
    Weighted experiment for numerical integration. It either targets the
    measure or lives on the reference cube. It is wrapped into an
    :class:`~openturns.ExperimentIntegration` rule. Ignored when an
    integration algorithm is set.
integrationAlgorithm : :class:`~openturns.IntegrationAlgorithm`, optional
    Integration algorithm used to compute the orthonormal basis. A rule
    built from a weighted experiment (e.g.
    :class:`~openturns.ExperimentIntegration`) exposes its nodes for a QR
    factorization; any other rule provides only integrals for a Cholesky
    factorization of the Gram matrix.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> dim = 2
>>> distribution = ot.Normal(dim)
>>> refBasis = ot.OrthogonalProductPolynomialFactory([ot.HermiteFactory()] * dim)
>>> basis = [refBasis.build(i) for i in range(6)]
>>> algo = otexp.BasisOrthonormalizationAlgorithm(basis, distribution)
>>> algo.run()
>>> phi0 = algo.getOrthonormalFunctions()[0]
)RAW"

%feature("docstring") OT::BasisOrthonormalizationAlgorithm::run
"Compute the orthonormal basis."

%feature("docstring") OT::BasisOrthonormalizationAlgorithm::setExperiment
"Accessor to the weighted experiment.

Setting an experiment resets the integration algorithm: the experiment,
wrapped into an :class:`~openturns.ExperimentIntegration`, becomes the
integration rule used to compute the Gram matrix. It must integrate
against the measure.

Parameters
----------
experiment : :class:`~openturns.WeightedExperiment`
    Weighted experiment."

%feature("docstring") OT::BasisOrthonormalizationAlgorithm::getExperiment
"Accessor to the weighted experiment.

Returns
-------
experiment : :class:`~openturns.WeightedExperiment`
    Weighted experiment. It is empty when an integration algorithm was set."

%feature("docstring") OT::BasisOrthonormalizationAlgorithm::setIntegrationAlgorithm
"Accessor to the integration algorithm.

Setting an integration algorithm resets the experiment. A rule built from
a weighted experiment (e.g. :class:`~openturns.ExperimentIntegration`) must
target the measure; any other rule integrates against the Lebesgue measure,
the density being accounted for internally.

Parameters
----------
integrationAlgorithm : :class:`~openturns.IntegrationAlgorithm`
    Integration algorithm."

%feature("docstring") OT::BasisOrthonormalizationAlgorithm::getIntegrationAlgorithm
"Accessor to the integration algorithm.

Returns
-------
integrationAlgorithm : :class:`~openturns.IntegrationAlgorithm`
    Integration algorithm."

%feature("docstring") OT::BasisOrthonormalizationAlgorithm::getOrthonormalFunctions
"Accessor to the orthonormal functions.

The orthonormal basis is computed on first access if it has not been
computed yet.

Returns
-------
functions : sequence of :class:`~openturns.Function`
    Orthonormal functions, ordered as the initial functions."

%feature("docstring") OT::BasisOrthonormalizationAlgorithm::getCoefficients
"Accessor to the coefficient matrix.

The orthonormal basis is computed on first access if it has not been
computed yet. Column :math:`j` of the matrix contains the coefficients of
the :math:`j`-th orthonormal function in the initial basis.

Returns
-------
coefficients : :class:`~openturns.SquareMatrix`
    Coefficients of the orthonormal functions in the initial basis."

%feature("docstring") OT::BasisOrthonormalizationAlgorithm::getMapping
"Accessor to the physical-to-reference mapping.

The orthonormal basis is computed on first access if it has not been
computed yet. The mapping sends the support of the measure to the
reference cube :math:`[-1, 1]^d`.

Returns
-------
mapping : :class:`~openturns.Function`
    Mapping from the support of the measure to the reference cube."

%feature("docstring") OT::BasisOrthonormalizationAlgorithm::setFunctionsCollection
"Accessor to the initial functions.

Each function must map :math:`\mathbb{R}^d` to :math:`\mathbb{R}`, where
:math:`d` is the dimension of the measure. Setting the functions invalidates
the previously computed basis.

Parameters
----------
functions : sequence of :class:`~openturns.Function`
    Initial functions."

%feature("docstring") OT::BasisOrthonormalizationAlgorithm::getFunctionsCollection
"Accessor to the initial functions.

Returns
-------
functions : sequence of :class:`~openturns.Function`
    Initial functions."

%feature("docstring") OT::BasisOrthonormalizationAlgorithm::setMeasure
"Accessor to the measure.

Setting the measure invalidates the previously computed basis.

Parameters
----------
measure : :class:`~openturns.Distribution`
    Measure with respect to which the functions are orthonormalized."

%feature("docstring") OT::BasisOrthonormalizationAlgorithm::getMeasure
"Accessor to the measure.

Returns
-------
measure : :class:`~openturns.Distribution`
    Measure with respect to which the functions are orthonormalized."
