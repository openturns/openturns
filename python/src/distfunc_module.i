// SWIG file distfunc_module.i

%module(package="openturns", docstring="Distribution special function.") DistFunc

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

%import base_module.i

/* Uncertainty/StatTests */
%include DistFunc.i

%pythoncode %{
try:
    from ._keyword_dispatch import apply_module_keywords as _keyword_apply_module
    _keyword_apply_module(__name__, globals())
    del _keyword_apply_module
except (ImportError, AttributeError):
    pass
%}
