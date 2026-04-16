// SWIG file FiniteDiscreteDistribution.i



%{
#include "openturns/FiniteDiscreteDistribution.hxx"
%}

%include FiniteDiscreteDistribution_doc.i

%include openturns/FiniteDiscreteDistribution.hxx

%copyctor OT::FiniteDiscreteDistribution;

%pythoncode %{
def UserDefined(*args):
    """Deprecated"""
    openturns.common.Log.Warn('class UserDefined is deprecated in favor of FiniteDiscreteDistribution')
    return openturns.dist_bundle3.FiniteDiscreteDistribution(*args)
%}
