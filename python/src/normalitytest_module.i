// SWIG file normalitytest_module.i

%module(package="openturns", docstring="Normality tests.") NormalityTest
#pragma SWIG nowarn=302,509

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

%import base_module.i

%import model_copula_module.i
%include UncertaintyModelCopulaTemplateDefs.i

/* Uncertainty/StatTests */
%include NormalityTest.i
