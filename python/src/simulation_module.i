// SWIG file simulation_module.i

%module(package="openturns", docstring="Simulation uncertainty propagation algorithms.") simulation
#pragma SWIG nowarn=302,509
//%feature("autodoc","1");

%{
#include "OTconfig.hxx"
#include "OTBase.hxx"
#include "OTModel.hxx"
#include "OTDistribution.hxx"
#include "OTTransformation.hxx"
#include "OTAnalytical.hxx"
#include "OTSimulation.hxx"
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

/* Uncertainty/Algorithm/Simulation */
%include SimulationResultImplementation.i
%include SimulationResult.i
%include Simulation.i
%include PostAnalyticalSimulation.i
%include Wilks.i
%include MonteCarlo.i
%include LHS.i
%include RandomizedLHS.i
%include ImportanceSampling.i
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
%include QuasiMonteCarlo.i
%include RandomizedQuasiMonteCarlo.i
%include QuasiMonteCarloResult.i
%include SimulationSensitivityAnalysis.i

/* At last we include template definitions */
%include UncertaintySimulationTemplateDefs.i
