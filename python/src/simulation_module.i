// SWIG file simulation_module.i

%module(package="openturns", docstring="EventSimulation uncertainty propagation algorithms.") simulation
#pragma SWIG nowarn=302,509
//%feature("autodoc","1");

%{
#include "openturns/OTconfig.hxx"
#include "openturns/OTBase.hxx"
#include "openturns/OTModel.hxx"
#include "openturns/OTDistribution.hxx"
#include "openturns/OTTransformation.hxx"
#include "openturns/OTAnalytical.hxx"
#include "openturns/OTSimulation.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

%import base_module.i

/* Uncertainty/Model */
/* Uncertainty/Distribution */
%import model_copula_module.i
%import UncertaintyModelCopulaTemplateDefs.i

/* Uncertainty/Algorithm/Transformation */
%import transformation_module.i
%import UncertaintyTransformationTemplateDefs.i

/* Uncertainty/Algorithm/Analytical */
%import analytical_module.i
%import UncertaintyAnalyticalTemplateDefs.i

/* Uncertainty/Algorithm/WeightedExperiment */
%import weightedexperiment_module.i

/* Uncertainty/Algorithm/Sensitivity */
%include ANCOVA.i
%include FAST.i
%include SobolIndicesImplementation.i
%include SobolIndices.i
%include MartinezSobolIndices.i
%include SobolIndicesAlgorithmImplementation.i
%include SobolIndicesAlgorithm.i
%include MartinezSensitivityAlgorithm.i
%include SobolIndicesExperiment.i

/* Uncertainty/Algorithm/EventSimulation */
%include SimulationResult.i
%include ProbabilitySimulationResult.i
%include SimulationAlgorithm.i
%include EventSimulation.i
%include ProbabilitySimulationAlgorithm.i
%include PostAnalyticalSimulation.i
%include Wilks.i
%include LHS.i
%include PostAnalyticalControlledImportanceSampling.i
%include PostAnalyticalImportanceSampling.i
%include RootStrategyImplementation.i
%include RootStrategy.i
%include SamplingStrategyImplementation.i
%include SamplingStrategy.i
%include DirectionalSampling.i
%include OrthogonalDirection.i
%include RandomDirection.i
%include MediumSafe.i
%include RiskyAndFast.i
%include SafeAndSlow.i
%include SimulationSensitivityAnalysis.i
%include SubsetSamplingResult.i
%include SubsetSampling.i
%include AdaptiveDirectionalSampling.i
%include ExpectationSimulationResult.i
%include ExpectationSimulationAlgorithm.i
%include SobolSimulationResult.i
%include SobolSimulationAlgorithm.i

/* At last we include template definitions */
%include UncertaintySimulationTemplateDefs.i
