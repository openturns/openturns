=============
Data analysis
=============

.. currentmodule:: openturns

Sample
======

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    Sample
    SamplePartition

.. _parametric_distribution_factories:

Building distributions from samples
===================================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    DistributionFactory
    DistributionFactoryResult
    DistributionFactoryLikelihoodResult
    ProfileLikelihoodResult
    CovariatesResult
    TimeVaryingResult

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
    PolyaFactory
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
    SmoothedUniformFactory
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
    StudentCopulaFactory

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
    FittingTest.ComputeKolmogorovStatistics
    FittingTest.Kolmogorov
    FittingTest.Lilliefors
    NormalityTest.AndersonDarlingNormal
    NormalityTest.CramerVonMisesNormal

Graphical tests
---------------

.. autosummary::
    :toctree: _generated/
    :template: functionWithPlot.rst_t

    VisualTest.DrawCDFplot
    VisualTest.DrawHenryLine
    VisualTest.DrawInsideOutside
    VisualTest.DrawKendallPlot
    VisualTest.DrawLinearModel
    VisualTest.DrawLinearModelResidual
    VisualTest.DrawLowerTailDependenceFunction
    VisualTest.DrawLowerExtremalDependenceFunction
    VisualTest.DrawPairs
    VisualTest.DrawPairsMarginals
    VisualTest.DrawPairsXY
    VisualTest.DrawParallelCoordinates
    VisualTest.DrawPPplot
    VisualTest.DrawQQplot
    VisualTest.DrawUpperTailDependenceFunction
    VisualTest.DrawUpperExtremalDependenceFunction

    :template: class.rst_t
    GeneralizedExtremeValueValidation
    GeneralizedParetoValidation

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

Iterative statistics
====================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    IterativeAlgorithm
    IterativeExtrema
    IterativeMoments
    IterativeThresholdExceedance

Quantile confidence
===================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    experimental.QuantileConfidence
