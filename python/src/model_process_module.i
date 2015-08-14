// SWIG file model_process_module.i

%module(package="openturns", docstring="Stochastic process.") model_process
#pragma SWIG nowarn=302,509
//%feature("autodoc","1");

%{
#include "OTconfig.hxx"
#include "OTBase.hxx"
#include "OTModel.hxx"
#include "OTDistribution.hxx"
#include "OTProcess.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

%import base_module.i
%import dist_bundle1_module.i
%import dist_bundle2_module.i
%import model_copula_module.i
%import randomvector_module.i
/* imports for ConditionedNormalProcess */
%import MetaModelResult.i
%import KrigingResult.i

/* Uncertainty/Process */
%include WhiteNoise.i
%include RandomWalk.i
%include ARMACoefficients.i
%include ARMAState.i
%include ARMA.i
%include SpectralNormalProcess.i
%include TemporalNormalProcess.i
%include AggregatedProcess.i
%include CompositeProcess.i
%include FunctionalBasisProcess.i
%include ARMAFactoryImplementation.i
%include WhittleFactoryState.i
%include WhittleFactory.i
%include ARMALikelihoodFactory.i
%include ARMAFactory.i
%include ConditionedNormalProcess.i

/* At last we include template definitions */
%include UncertaintyProcessTemplateDefs.i
