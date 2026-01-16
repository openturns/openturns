// SWIG file experimental_module.i

%module(package="openturns", docstring="Experimental features.") experimental
#pragma SWIG nowarn=302,509

%{
#include "openturns/OTconfig.hxx"
#include "openturns/OTBase.hxx"
#include "openturns/OTModel.hxx"
#include "openturns/OTDistribution.hxx"
#include "openturns/OTProcess.hxx"
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

/* Base/Optim */
%import optim_module.i
%include LinearProblem.i
%include HiGHS.i

/* Base/Solver */
%include LeastSquaresEquationsSolver.i

%include ConstantEvaluation.i
%include ConstantFunction.i
%include VertexFieldToPointFunction.i

/* Uncertainty/Model */

/* Uncertainty/Distribution */
%import model_copula_module.i
%import dist_bundle2_module.i
%import dist_bundle3_module.i

/* Uncertainty/Algorithm/OrthogonalBasis */
%import orthogonalbasis_module.i
%import UncertaintyOrthogonalBasisTemplateDefs.i
%include FiniteOrthogonalFunctionFactory.i

/* Uncertainty/Algorithm/WeightedExperiment */
%import weightedexperiment_module.i
%include UncertaintyWeightedExperimentTemplateDefs.i
%include FejerExperiment.i

/* Uncertainty */
%import metamodel_module.i
%include simulation_module.i

/* Uncertainty/Process */
%import model_process_module.i

/* Uncertainty/Algorithm */
%include SequentialSamplingAlgorithmImplementation.i
%include SequentialSamplingAlgorithm.i
%include LOLAVoronoi.i
%include QuantileConfidence.i
%include LineSampling.i

/* Uncertainty/Distribution */
%include CombinationsDistribution.i
%include MarginalUniformOrderStatistics.i

/* Uncertainty/Algorithm/Metamodel */
%include LinearModelValidation.i
%include EfficientGlobalOptimization.i
%include GaussianProcessRegressionCrossValidation.i

/* Uncertainty/Model */
%include RatioOfUniforms.i
%include GaussianProcessRandomVector.i

/* Uncertainty/Process */
%include ConditionedGaussianProcess.i
