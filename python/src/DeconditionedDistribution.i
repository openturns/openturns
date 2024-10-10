// SWIG file DeconditionedDistribution.i

%{
#include "openturns/DeconditionedDistribution.hxx"
%}

%include DeconditionedDistribution_doc.i

%copyctor OT::DeconditionedDistribution;

%include openturns/DeconditionedDistribution.hxx

%pythoncode %{
def ConditionalDistribution(conditionedDistribution, conditioningDistribution, linkFunction=None):
    """
    Retro-compatibility alias for DeconditionedDistribution.
    """
    openturns.common.Log.Warn('class ConditionalDistribution is deprecated in favor of DeconditionedDistribution')
    if linkFunction is None:
        linkFunction = openturns.func.IdentityFunction(conditioningDistribution.getDimension())
    return openturns.dist_bundle2.DeconditionedDistribution(conditionedDistribution, conditioningDistribution, linkFunction)
%}
