// SWIG file FiniteDiscreteDistributionFactory.i

%{
#include "openturns/FiniteDiscreteDistributionFactory.hxx"
%}

%include FiniteDiscreteDistributionFactory_doc.i

%copyctor OT::FiniteDiscreteDistributionFactory;

%include openturns/FiniteDiscreteDistributionFactory.hxx

%pythoncode %{
def UserDefinedFactory(*args):
    """Deprecated"""
    openturns.common.Log.Warn('class UserDefinedFactory is deprecated in favor of FiniteDiscreteDistributionFactory')
    return openturns.dist_bundle3.FiniteDiscreteDistributionFactory(*args)
%}
