=======================
Probabilistic modelling
=======================

.. currentmodule:: openturns

.. _parametric_distributions:

Continuous parametric distributions
===================================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    Distribution
    DistributionCollection
    PythonDistribution
    ChaospyDistribution
    SciPyDistribution

.. autosummary::
    :toctree: _generated/
    :template: Distribution.rst_t

    Arcsine

    :template: classWithPlot.rst_t
    Beta
    Burr

    :template: Distribution.rst_t
    Chi
    ChiSquare

    :template: classWithPlot.rst_t
    Dirichlet

    :template: Distribution.rst_t
    Epanechnikov
    Exponential

    :template: classWithPlot.rst_t
    FisherSnedecor

    :template: Distribution.rst_t
    Frechet

    :template: classWithPlot.rst_t
    Gamma
    GeneralizedExtremeValue
    GeneralizedPareto

    :template: Distribution.rst_t
    Gumbel
    Histogram
    InverseChiSquare
    InverseGamma
    InverseNormal
    InverseWishart
    KPermutationsDistribution
    Laplace
    Logistic
    LogNormal
    LogUniform
    MeixnerDistribution
    NonCentralChiSquare
    NonCentralStudent
    Normal

    :template: classWithPlot.rst_t
    NormalGamma

    :template: Distribution.rst_t
    Pareto
    Rayleigh
    Rice
    SmoothedUniform
    SquaredNormal
    Student
    Trapezoidal
    Triangular
    TruncatedNormal
    Uniform
    VonMises
    WeibullMin
    WeibullMax
    Wishart

Discrete parametric distributions
=================================

.. autosummary::
    :toctree: _generated/
    :template: Distribution.rst_t

    Bernoulli
    Binomial
    Dirac
    Geometric
    Hypergeometric
    Multinomial

    :template: classWithPlot.rst_t
    NegativeBinomial

    :template: Distribution.rst_t
    Poisson
    Skellam
    UserDefined
    ZipfMandelbrot

Parametrized distributions
==========================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    ParametrizedDistribution
    DistributionParameters
    ArcsineMuSigma
    BetaMuSigma
    GammaMuSigma
    GumbelLambdaGamma
    GumbelMuSigma
    LogNormalMuSigma
    LogNormalMuSigmaOverMu
    WeibullMaxMuSigma
    WeibullMinMuSigma

Pseudo-random numbers generator
===============================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    RandomGenerator
    RandomGeneratorState

Combining and transforming distributions
========================================

.. autosummary::
    :toctree: _generated/
    :template: classWithPlot.rst_t

    BayesDistribution

    :template: Distribution.rst_t
    ComposedDistribution

    :template: classWithPlot.rst_t
    CompositeDistribution

    :template: Distribution.rst_t
    ConditionalDistribution
    CumulativeDistributionNetwork
    DiscreteCompoundDistribution
    MaximumDistribution

    :template: classWithPlot.rst_t
    MaximumEntropyOrderStatisticsDistribution

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    OrderStatisticsMarginalChecker
    PosteriorDistribution

.. autosummary::
    :toctree: _generated/
    :template: Distribution.rst_t

    ProductDistribution
    RandomMixture

.. autosummary::
    :toctree: _generated/
    :template: Distribution.rst_t

    TruncatedDistribution
    KernelMixture

    :template: classWithPlot.rst_t
    Mixture

.. _copulas:

Copulas
=======

Refer to :ref:`copula`.

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    ArchimedeanCopula

.. autosummary::
    :toctree: _generated/
    :template: Copula.rst_t

    AliMikhailHaqCopula
    ClaytonCopula
    FarlieGumbelMorgensternCopula
    FrankCopula
    GumbelCopula
    IndependentCopula
    EmpiricalBernsteinCopula
    ExtremeValueCopula
    MaximumEntropyOrderStatisticsCopula

    :template: class.rst_t
    MinCopula

    :template: Copula.rst_t
    NormalCopula


Combining and transforming copulas
==================================

.. autosummary::
    :toctree: _generated/
    :template: classWithPlot.rst_t

    ComposedCopula

    :template: Copula.rst_t
    SklarCopula

    :template: classWithPlot.rst_t
    OrdinalSumCopula

Random vectors
==============

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    RandomVector
    PythonRandomVector

    :template: classWithPlot.rst_t
    CompositeRandomVector
    ConditionalRandomVector

    :template: class.rst_t
    ConstantRandomVector

    :template: classWithPlot.rst_t
    UsualRandomVector

Low-level distribution functions
================================

.. autosummary::
    :toctree: _generated/
    :template: function.rst_t

    DistFunc_dBinomial
    DistFunc_dHypergeometric
    DistFunc_dPoisson
    DistFunc_logdBinomial
    DistFunc_logdHypergeometric
    DistFunc_logdPoisson
    DistFunc_pHypergeometric
    DistFunc_pNormal
    DistFunc_qNormal
    DistFunc_rBinomial
    DistFunc_rHypergeometric
    DistFunc_rNormal
    DistFunc_rPoisson
    DistFunc_kFactor
    DistFunc_kFactorPooled

