// SWIG file stattests_module.i

%module(package="openturns", docstring="Statistical tests.") stattests
#pragma SWIG nowarn=302,509

%{
#include "openturns/OTconfig.hxx"
#include "openturns/OTBase.hxx"
#include "openturns/OTModel.hxx"
#include "openturns/OTDistribution.hxx"
#include "openturns/OTStatTests.hxx"
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

/* Uncertainty/StatTests */
%include DickeyFullerTest.i

/* At last we include template definitions */
%include UncertaintyStatTestsTemplateDefs.i
