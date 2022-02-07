// SWIG file model_process_module.i

%module(package="openturns", docstring="Stochastic process.") model_process
#pragma SWIG nowarn=302,509
//%feature("autodoc","1");

%{
#include "openturns/OTconfig.hxx"
#include "openturns/OTBase.hxx"
#include "openturns/OTModel.hxx"
#include "openturns/OTDistribution.hxx"
#include "openturns/OTProcess.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

%include BaseFuncCollection.i
%import base_module.i
%import dist_bundle1_module.i
%import dist_bundle2_module.i
%import dist_bundle3_module.i
%import model_copula_module.i
%import randomvector_module.i
%import weightedexperiment_module.i
/* imports for ConditionedGaussianProcess */
%import MetaModelResult.i
%import KrigingResult.i

/* Uncertainty/Process */
%include WhiteNoise.i
%include RandomWalk.i
%include DiscreteMarkovChain.i
%include ARMACoefficients.i
%include ARMAState.i
%include ARMA.i
%include SpectralGaussianProcess.i
%include GaussianProcess.i
%include AggregatedProcess.i
%include CompositeProcess.i
%include FunctionalBasisProcess.i
%include ARMAFactoryImplementation.i
%include WhittleFactoryState.i
%include WhittleFactory.i
%include ARMALikelihoodFactory.i
%include ARMAFactory.i
%include ConditionedGaussianProcess.i
%include KarhunenLoeveQuadratureAlgorithm.i
%include KarhunenLoeveSVDAlgorithm.i

/* At last we include template definitions */
%include UncertaintyProcessTemplateDefs.i
