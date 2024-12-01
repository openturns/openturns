// SWIG file experimental_module.i

%module(package="openturns", docstring="Experimental features.") experimental
#pragma SWIG nowarn=302,509

%{
#include "openturns/OTconfig.hxx"
#include "openturns/OTBase.hxx"
#include "openturns/OTModel.hxx"
#include "openturns/OTDistribution.hxx"
#include "openturns/OTWeightedExperiments.hxx"
#include "openturns/OTOrthogonalBasis.hxx"
#include "openturns/OTMetaModel.hxx"
#include "openturns/OTSimulation.hxx"
#include "openturns/OTSolver.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

/* Base/Type */
%include BaseTypCollection.i
%import typ_module.i

/* Base/Func */
%include BaseFuncCollection.i
%import base_module.i

/* Base/Geom */
%include BoundaryMesher.i

/* Base/Optim */
%import optim_module.i

/* Base/Stat */
%include LatentVariableModel.i
%include SamplePartition.i

/* Base/Solver */
%include LeastSquaresEquationsSolver.i

/* Uncertainty/Model */

/* Uncertainty/Distribution */
%import model_copula_module.i
%import dist_bundle3_module.i

/* Uncertainty/Algorithm/OrthogonalBasis */
%import orthogonalbasis_module.i
%import UncertaintyOrthogonalBasisTemplateDefs.i

/* Uncertainty/Algorithm/WeightedExperiment */
%import weightedexperiment_module.i
%include UncertaintyWeightedExperimentTemplateDefs.i
%include ExperimentIntegration.i
%include FejerExperiment.i

/* Uncertainty/Algorith/Optimization */
%include SequentialSamplingAlgorithmImplementation.i
%include SequentialSamplingAlgorithm.i
%include LOLAVoronoi.i

/* Uncertainty */
%import bayesian_module.i
%import metamodel_module.i
%include simulation_module.i

/* Base/Algo */
%include CubaIntegration.i

/* Uncertainty/Algorithm/Metamodel */
%include UserDefinedMetropolisHastings.i
%include FieldFunctionalChaosResult.i
%include FieldToPointFunctionalChaosAlgorithm.i
%include FieldFunctionalChaosSobolIndices.i
%include PointToFieldFunctionalChaosAlgorithm.i

/* Uncertainty/Algorithm */
%include SequentialSamplingAlgorithmImplementation.i
%include SequentialSamplingAlgorithm.i
%include LOLAVoronoi.i

/*Uncertainty/Algorithm/Sensitivity */
%include RankSobolSensitivityAlgorithm.i

/* Uncertainty/Distribution */
%include CombinationsDistribution.i
%include PointConditionalDistribution.i
%include SmoothedUniformFactory.i
%include PosteriorDistribution.i
%include UniformOrderStatistics.i
%include GeneralizedParetoValidation.i

/* Uncertainty/Algorithm/Metamodel */
%include FunctionalChaosValidation.i
%include LinearModelValidation.i
%include GaussianProcessFitterResult.i
%include GaussianProcessFitter.i
%include GaussianProcessRegressionResult.i
%include GaussianProcessRegression.i
%include GaussianProcessConditionalCovariance.i

 /* Uncertainty/Model */
%include RatioOfUniforms.i
