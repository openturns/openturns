// SWIG file linearmodeltest_module.i

%module(package="openturns", docstring="Linear model tests.") LinearModelTest

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

%import metamodel_module.i
%import UncertaintyMetaModelTemplateDefs.i

/* Uncertainty/StatTests */
%include LinearModelTest.i

%pythoncode %{
try:
    from ._keyword_dispatch import apply_module_keywords as _keyword_apply_module
    _keyword_apply_module(__name__, globals())
    del _keyword_apply_module
except (ImportError, AttributeError):
    pass
%}
