// SWIG file classification_module.i

%module(package="openturns", docstring="Classification algorithms.") classification
#pragma SWIG nowarn=302,509
//%feature("autodoc","1");

%{
#include "openturns/OTconfig.hxx"
#include "openturns/OTBase.hxx"
#include "openturns/OTModel.hxx"
#include "openturns/OTClassification.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

%import base_module.i

/* Uncertainty/Model */
/* Uncertainty/Distribution */
%import  model_copula_module.i
%import UncertaintyModelCopulaTemplateDefs.i
//%import randomvector_module.i
%import dist_bundle1_module.i
%import dist_bundle2_module.i

/* Uncertainty/Algorithm/Classification */
%include MixtureClassifier.i
%include MinimumVolumeClassifier.i 

/* At last we include template definitions */
%include UncertaintyClassificationTemplateDefs.i
