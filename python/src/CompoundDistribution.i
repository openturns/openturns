// SWIG file CompoundDistribution.i

%{
#include "openturns/CompoundDistribution.hxx"
%}

%include CompoundDistribution_doc.i

%copyctor OT::CompoundDistribution;

%include openturns/CompoundDistribution.hxx

%pythoncode %{
def DeconditionedDistribution(conditionedDistribution, conditioningDistribution, linkFunction=None):
    """
    Retro-compatibility alias for CompoundDistribution.
    """
    openturns.common.Log.Warn('class DeconditionedDistribution is deprecated in favor of CompoundDistribution')
    if linkFunction is None:
        linkFunction = openturns.func.IdentityFunction(conditioningDistribution.getDimension())
    return openturns.dist_bundle2.CompoundDistribution(conditionedDistribution, conditioningDistribution, linkFunction)
%}
