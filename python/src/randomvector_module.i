// SWIG file randomvector_module.i

%module(package="openturns", docstring="Random vectors.") randomvector


%{
#include "openturns/OTconfig.hxx"
#include "openturns/OTBase.hxx"
#include "openturns/OTAlgorithm.hxx"
#include "openturns/OTModel.hxx"
#include "openturns/OTDistribution.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

%import base_module.i
%import model_copula_module.i

/* Uncertainty/Model */
%include ProcessImplementation.i
%include Process.i
%include RandomVectorImplementation.i
%include ConstantRandomVector.i
%include UsualRandomVector.i
%include RatioOfUniforms.i

/* At last we include template definitions */
%include UncertaintyRandomVectorTemplateDefs.i
