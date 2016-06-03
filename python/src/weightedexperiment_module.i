// SWIG file weightedexperiment_module.i

%module(package="openturns", docstring="Weighted designs of experiments.") weightedexperiment
#pragma SWIG nowarn=302,509
//%feature("autodoc","1");

%{
#include "openturns/OTconfig.hxx"
#include "openturns/OTBase.hxx"
#include "openturns/OTModel.hxx"
#include "openturns/OTWeightedExperiments.hxx"
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

/* Uncertainty/Algorithm/Experiment */
%include WeightedExperimentImplementation.i
%include WeightedExperiment.i
%include BootstrapExperiment.i
%include FixedExperiment.i
%include GaussProductExperiment.i
%include ImportanceSamplingExperiment.i
%include LHSExperiment.i
%include LowDiscrepancyExperiment.i
%include MonteCarloExperiment.i

/* At last we include template definitions */
%include UncertaintyWeightedExperimentTemplateDefs.i
