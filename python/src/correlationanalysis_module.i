// SWIG file normalitytest_module.i

%module(package="openturns", docstring="Correlation analysis.") CorrelationAnalysis
#pragma SWIG nowarn=302,509

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

%import base_module.i

/* Uncertainty/StatTests */
%include CorrelationAnalysis.i
