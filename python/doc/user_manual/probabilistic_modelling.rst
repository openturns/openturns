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
    SciPyDistribution

.. autosummary::
    :toctree: _generated/
    :template: Distribution.rst_t

    Arcsine
    Beta
    Burr
    Chi
    ChiSquare
    Dirichlet
    Epanechnikov
    Exponential
    FisherSnedecor
    Frechet
    Gamma
    GeneralizedExtremeValue
    GeneralizedPareto
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
    NormalGamma
    Rayleigh
    Rice
    Student
    Trapezoidal
    Triangular
    TruncatedNormal
    Uniform
    VonMises
    Weibull
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
    Multinomial
    NegativeBinomial
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
    GumbelMuSigma
    GumbelAB
    LogNormalMuSigma
    LogNormalMuSigmaOverMu
    WeibullMuSigma

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
    :template: Distribution.rst_t

    BayesDistribution
    ComposedDistribution

    :template: classWithPlot.rst_t
    CompositeDistribution

    :template: Distribution.rst_t
    ConditionalDistribution
    CumulativeDistributionNetwork
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

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

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

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    Copula
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

    :template: classWithPlot.rst_t
    MaximumEntropyOrderStatisticsCopula

    :template: class.rst_t
    MinCopula
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
    CompositeRandomVector

    :template: classWithPlot.rst_t
    ConditionalRandomVector

    :template: class.rst_t
    ConstantRandomVector
    UsualRandomVector

Samplers
========

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    MCMC
    RandomWalkMetropolisHastings
    CalibrationStrategy

Low-level distribution functions
================================

.. autosummary::
    :toctree: _generated/
    :template: function.rst_t

    DistFunc_pNormal
    DistFunc_qNormal
    DistFunc_rNormal
    DistFunc_kFactor
    DistFunc_kFactorPooled

