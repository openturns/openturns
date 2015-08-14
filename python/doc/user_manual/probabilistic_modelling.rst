=======================
Probabilistic modelling
=======================

.. currentmodule:: openturns

Parametric distributions
========================

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
    Bernoulli
    Beta
    Binomial
    Burr
    Chi
    ChiSquare
    Dirac
    Dirichlet
    Epanechnikov
    Exponential
    FisherSnedecor
    Frechet
    Gamma
    GeneralizedPareto
    Geometric
    Gumbel
    Histogram
    InverseChiSquare
    InverseGamma
    InverseNormal
    KPermutationsDistribution
    Laplace
    Logistic
    LogNormal
    LogUniform
    MeixnerDistribution
    Multinomial
    NegativeBinomial
    NonCentralChiSquare
    NonCentralStudent
    Normal
    NormalGamma
    Poisson
    Rayleigh
    Rice
    Skellam
    Student
    Trapezoidal
    Triangular
    TruncatedNormal
    Uniform
    UserDefined
    VonMises
    Weibull
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

Combining and transforming distributions
========================================

.. autosummary::
    :toctree: _generated/
    :template: Distribution.rst_t

    BayesDistribution
    ComposedDistribution
    CompositeDistribution
    ConditionalDistribution
    MaximumEntropyOrderStatisticsDistribution

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t
    
    OrderStatisticsMarginalChecker

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
    Mixture


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
    MaximumEntropyOrderStatisticsCopula
    MinCopula
    NormalCopula


Combining and transforming copulas
==================================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    ComposedCopula

.. autosummary::
    :toctree: _generated/
    :template: Copula.rst_t

    SklarCopula
    OrdinalSumCopula

Random vectors
==============

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    RandomVector
    PythonRandomVector
    CompositeRandomVector
    ConditionalRandomVector
    ConstantRandomVector
    PosteriorRandomVector
    UsualRandomVector

Samplers
========

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    Sampler
    MCMC
    RandomWalkMetropolisHastings
    CalibrationStrategy
