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

.. _parametric_distibution_factories:

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
    NormalCopulaFactory
    PlackettCopulaFactory

Correlation analysis
====================

.. autosummary::
    :toctree: _generated/
    :template: function.rst_t

    CorrelationAnalysis_PearsonCorrelation
    CorrelationAnalysis_SpearmanCorrelation
    CorrelationAnalysis_PCC
    CorrelationAnalysis_PRCC
    CorrelationAnalysis_SRC
    CorrelationAnalysis_SRRC
    CorrelationAnalysis_SignedSRC

Sensitivity Analysis
====================

Refer to :ref:`sensitivity_sobol`.

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

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

    FittingTest_AIC
    FittingTest_AICC
    FittingTest_BIC
    FittingTest_ChiSquared
    FittingTest_Kolmogorov
    FittingTest_Lilliefors
    NormalityTest_AndersonDarlingNormal
    NormalityTest_CramerVonMisesNormal

Graphical tests
---------------

.. autosummary::
    :toctree: _generated/
    :template: functionWithPlot.rst_t

    VisualTest_DrawPairs
    VisualTest_DrawPairsMarginals
    VisualTest_DrawCobWeb
    VisualTest_DrawHenryLine
    VisualTest_DrawKendallPlot
    VisualTest_DrawLinearModel
    VisualTest_DrawLinearModelResidual
    VisualTest_DrawQQplot
    VisualTest_DrawCDFplot

Hypothesis tests
----------------

.. autosummary::
    :toctree: _generated/
    :template: function.rst_t

    HypothesisTest_ChiSquared
    HypothesisTest_FullPearson
    HypothesisTest_FullSpearman
    HypothesisTest_PartialPearson
    HypothesisTest_PartialSpearman
    HypothesisTest_Pearson
    HypothesisTest_Spearman
    HypothesisTest_TwoSamplesKolmogorov

Linear model tests
------------------

.. autosummary::
    :toctree: _generated/
    :template: function.rst_t

    LinearModelTest_LinearModelFisher
    LinearModelTest_LinearModelResidualMean
    LinearModelTest_LinearModelHarrisonMcCabe
    LinearModelTest_LinearModelBreuschPagan
    LinearModelTest_LinearModelDurbinWatson
    LinearModelTest_FullRegression
    LinearModelTest_PartialRegression

Model selection
---------------

.. autosummary::
    :toctree: _generated/
    :template: function.rst_t

    FittingTest_BestModelAIC
    FittingTest_BestModelAICC
    FittingTest_BestModelBIC
    FittingTest_BestModelChiSquared
    FittingTest_BestModelKolmogorov
    FittingTest_BestModelLilliefors


