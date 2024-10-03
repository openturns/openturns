// SWIG file JointByConditioningDistribution.i

%{
#include "openturns/JointByConditioningDistribution.hxx"
%}

%include JointByConditioningDistribution_doc.i

%copyctor OT::JointByConditioningDistribution;

%include openturns/JointByConditioningDistribution.hxx

%pythoncode %{
def BayesDistribution(conditionedDistribution, conditioningDistribution, linkFunction=None):
    openturns.common.Log.Warn('class BayesDistribution is deprecated in favor of JointByConditioningDistribution')
    if linkFunction is None:
        linkFunction = openturns.func.IdentityFunction(conditioningDistribution.getDimension())
    return openturns.dist_bundle2.JointByConditioningDistribution(conditionedDistribution, conditioningDistribution, linkFunction)
%}
