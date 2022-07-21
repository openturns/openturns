// SWIG file specfunc_module.i

%module(package="openturns", docstring="Special function.") SpecFunc
#pragma SWIG nowarn=302,509

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

%import base_module.i

/* Uncertainty/StatTests */
%include SpecFunc.i
