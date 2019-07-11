// SWIG file WeibullMinFactory.i

%{
#include "openturns/WeibullMinFactory.hxx"
%}

%include WeibullMinFactory_doc.i

%include openturns/WeibullMinFactory.hxx
namespace OT { %extend WeibullMinFactory { WeibullMinFactory(const WeibullMinFactory & other) { return new OT::WeibullMinFactory(other); } } }

%pythoncode %{
def WeibullFactory():
    openturns.common.Log.Warn('class WeibullFactory is deprecated in favor of WeibullMinFactory')
    return openturns.dist_bundle3.WeibullMinFactory()
%}
