// SWIG file distfunc_module.i

%module(package="openturns", docstring="Distribution special function.") DistFunc
#pragma SWIG nowarn=302,509

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

%import base_module.i

/* Uncertainty/StatTests */
%include DistFunc.i
