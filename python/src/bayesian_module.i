// SWIG file bayesian_module.i

%module(package="openturns", docstring="Bayesian algorithms.") bayesian
//%feature("autodoc","1");

%{
#include "openturns/OTconfig.hxx"
#include "openturns/OTBase.hxx"
#include "openturns/OTModel.hxx"
#include "openturns/OTBayesian.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

%import base_module.i

/* Uncertainty/Model */
/* Uncertainty/Distribution */
%include UncertaintyModelCopulaCollection.i
%import model_copula_module.i
%import UncertaintyModelCopulaTemplateDefs.i
%import dist_bundle1_module.i
%import dist_bundle2_module.i

/* Uncertainty/Model */
%import randomvector_module.i

/* Uncertainty/Bayesian */
%include CalibrationResult.i
%include CalibrationAlgorithmImplementation.i
%include CalibrationAlgorithm.i
%include GaussianLinearCalibration.i
%include LinearLeastSquaresCalibration.i
%include MetropolisHastingsImplementation.i
%include MetropolisHastings.i
%include NonLinearLeastSquaresCalibration.i
%include GaussianNonLinearCalibration.i
%include RandomVectorMetropolisHastings.i
%include RandomWalkMetropolisHastings.i
%include IndependentMetropolisHastings.i
%include UserDefinedMetropolisHastings.i
%include Gibbs.i

/* At last we include template definitions */
%include UncertaintyBayesianTemplateDefs.i
