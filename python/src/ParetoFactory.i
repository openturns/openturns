// SWIG file ParetoFactory.i

%{
#include "openturns/ParetoFactory.hxx"
%}

%include ParetoFactory_doc.i

%include openturns/ParetoFactory.hxx
namespace OT { %extend ParetoFactory { ParetoFactory(const ParetoFactory & other) { return new OT::ParetoFactory(other); } } }

%pythoncode %{
def WeibullFactory():
    openturns.common.Log.Warn('class WeibullFactory is deprecated in favor of ParetoFactory')
    return openturns.dist_bundle3.ParetoFactory()
%}
