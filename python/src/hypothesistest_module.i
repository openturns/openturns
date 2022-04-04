// SWIG file hypothesistest_module.i

%module(package="openturns", docstring="Hypothesis tests.") HypothesisTest
#pragma SWIG nowarn=302,509

%{
#include "openturns/OTStatTests.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

%import base_module.i

%import model_copula_module.i
%include UncertaintyModelCopulaTemplateDefs.i

/* Uncertainty/StatTests */
%include HypothesisTest.i
