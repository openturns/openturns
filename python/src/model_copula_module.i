// SWIG file model_copula_module.i

%module(package="openturns", docstring="Copulas.") model_copula

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

/* Uncertainty/Model */
%include DistributionImplementation.i
%include Distribution.i
%include DistributionFactoryResult.i
%include DistributionParametersImplementation.i
%include DistributionParameters.i
%include DistributionFactoryImplementation.i
%include DistributionFactory.i
%include EllipticalDistribution.i
%include ArchimedeanCopula.i
%include SklarCopula.i

/* Uncertainty/Distribution (copulas) */
%include BlockIndependentDistribution.i
%include JointDistribution.i
%include CumulativeDistributionNetwork.i
%include BlockIndependentCopula.i
%include OrdinalSumCopula.i

/* At last we include template definitions */
%include UncertaintyModelCopulaTemplateDefs.i

%pythoncode %{
try:
    from ._keyword_dispatch import apply_module_keywords as _keyword_apply_module
    _keyword_apply_module(__name__, globals())
    del _keyword_apply_module
except (ImportError, AttributeError):
    pass
%}
