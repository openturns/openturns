====================
Statistics on sample
====================

.. currentmodule:: openturns

Sample
======

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    Sample

.. _parametric_distribution_factories:

Building distributions from samples
===================================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    DistributionFactory
    DistributionFactoryResult

    :template: DistributionFactory.rst_t

    ArcsineFactory
    BernoulliFactory
    BetaFactory
    BinomialFactory
    BurrFactory
    ChiFactory
    ChiSquareFactory
    DiracFactory
    DirichletFactory
    ExponentialFactory
    FisherSnedecorFactory
    FrechetFactory
    GammaFactory
    GeneralizedExtremeValueFactory
    GeneralizedParetoFactory
    GeometricFactory
    GumbelFactory
    HistogramFactory
    InverseNormalFactory

    :template: classWithPlot.rst_t

    KernelSmoothing

    :template: DistributionFactory.rst_t

    LaplaceFactory

    :template: class.rst_t

    LeastSquaresDistributionFactory

    :template: DistributionFactory.rst_t

    LogisticFactory
    LogNormalFactory
    LogUniformFactory

    :template: class.rst_t

    MaximumLikelihoodFactory

    :template: DistributionFactory.rst_t

    MeixnerDistributionFactory

    :template: class.rst_t

    MethodOfMomentsFactory

    :template: DistributionFactory.rst_t

    MultinomialFactory
    NegativeBinomialFactory
    NormalFactory
    ParetoFactory
    PoissonFactory

    :template: class.rst_t

    QuantileMatchingFactory

    :template: DistributionFactory.rst_t

    RayleighFactory

    :template: class.rst_t

    RiceFactory

    :template: DistributionFactory.rst_t

    SkellamFactory
    StudentFactory
    TrapezoidalFactory
    TriangularFactory
    TruncatedNormalFactory
    UniformFactory
    UserDefinedFactory
    VonMisesFactory
    WeibullMinFactory
    WeibullMaxFactory

Building copulas from samples
=============================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    AliMikhailHaqCopulaFactory
    BernsteinCopulaFactory

    :template: DistributionFactory.rst_t

    ClaytonCopulaFactory

    :template: class.rst_t

    FarlieGumbelMorgensternCopulaFactory

    :template: DistributionFactory.rst_t

    FrankCopulaFactory
    GumbelCopulaFactory
    IndependentCopulaFactory
    NormalCopulaFactory
    PlackettCopulaFactory

Sensitivity Analysis
====================

Refer to :ref:`sensitivity_sobol`.

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    CorrelationAnalysis
    ANCOVA
    FAST

    :template: classWithPlot.rst_t

    SobolIndicesAlgorithm
    MartinezSensitivityAlgorithm
    SaltelliSensitivityAlgorithm
    JansenSensitivityAlgorithm
    MauntzKucherenkoSensitivityAlgorithm

    :template: class.rst_t

    SobolIndicesExperiment

    :template: classWithPlot.rst_t

    SobolSimulationAlgorithm
    SobolSimulationResult

HSIC Indices
============

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    HSICStat
    HSICUStat
    HSICVStat
    HSICEstimator
    HSICEstimatorConditionalSensitivity
    HSICEstimatorGlobalSensitivity
    HSICEstimatorTargetSensitivity

Statistical tests
=================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    TestResult

Goodness-of-fit metrics & tests
-------------------------------

.. autosummary::
    :toctree: _generated/
    :template: function.rst_t

    FittingTest.AIC
    FittingTest.AICC
    FittingTest.BIC
    FittingTest.ChiSquared
    FittingTest.Kolmogorov
    FittingTest.Lilliefors
    NormalityTest.AndersonDarlingNormal
    NormalityTest.CramerVonMisesNormal

Graphical tests
---------------

.. autosummary::
    :toctree: _generated/
    :template: functionWithPlot.rst_t

    VisualTest.DrawPairs
    VisualTest.DrawPairsMarginals
    VisualTest.DrawParallelCoordinates
    VisualTest.DrawHenryLine
    VisualTest.DrawKendallPlot
    VisualTest.DrawLinearModel
    VisualTest.DrawLinearModelResidual
    VisualTest.DrawQQplot
    VisualTest.DrawCDFplot

Hypothesis tests
----------------

.. autosummary::
    :toctree: _generated/
    :template: function.rst_t

    HypothesisTest.ChiSquared
    HypothesisTest.LikelihoodRatioTest
    HypothesisTest.FullPearson
    HypothesisTest.FullSpearman
    HypothesisTest.PartialPearson
    HypothesisTest.PartialSpearman
    HypothesisTest.Pearson
    HypothesisTest.Spearman
    HypothesisTest.TwoSamplesKolmogorov

Linear model tests
------------------

.. autosummary::
    :toctree: _generated/
    :template: function.rst_t

    LinearModelTest.LinearModelFisher
    LinearModelTest.LinearModelResidualMean
    LinearModelTest.LinearModelHarrisonMcCabe
    LinearModelTest.LinearModelBreuschPagan
    LinearModelTest.LinearModelDurbinWatson
    LinearModelTest.FullRegression
    LinearModelTest.PartialRegression

Model selection
---------------

.. autosummary::
    :toctree: _generated/
    :template: function.rst_t

    FittingTest.BestModelAIC
    FittingTest.BestModelAICC
    FittingTest.BestModelBIC
    FittingTest.BestModelChiSquared
    FittingTest.BestModelKolmogorov
    FittingTest.BestModelLilliefors
