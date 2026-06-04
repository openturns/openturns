// SWIG file LinearCombinationDistribution.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & kolmogorovNormOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & kolmogorovNormOut %{ $result = OT::AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"),  SWIG_POINTER_OWN)); %}

%{
#include "openturns/LinearCombinationDistribution.hxx"
%}
%include LinearCombinationDistribution_doc.i

%copyctor OT::LinearCombinationDistribution;

%include openturns/LinearCombinationDistribution.hxx

%pythoncode %{
def RandomMixture(*args):
    """Deprecated"""
    openturns.common.Log.Warn('class RandomMixture is deprecated in favor of LinearCombinationDistribution')
    return openturns.dist_bundle3.LinearCombinationDistribution(*args)
%}
