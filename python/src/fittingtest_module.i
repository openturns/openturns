// SWIG file fittingtest_module.i

%module(package="openturns", docstring="FittingTest tests.") FittingTest
#pragma SWIG nowarn=302,509

%{
#include "openturns/OTStatTests.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

%import base_module.i

%include UncertaintyModelCopulaCollection.i
%import model_copula_module.i
%include UncertaintyModelCopulaTemplateDefs.i

/* Uncertainty/StatTests */
%include FittingTest.i
