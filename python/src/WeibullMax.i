// SWIG file WeibullMax.i

%{
#include "openturns/WeibullMax.hxx"
%}

%include WeibullMax_doc.i

%include openturns/WeibullMax.hxx
namespace OT { %extend WeibullMax { WeibullMax(const WeibullMax & other) { return new OT::WeibullMax(other); } } }

%pythoncode %{
def Weibull(alpha=1.0, beta=1.0, gamma=0.0):
    openturns.common.Log.Warn('class Weibull is deprecated in favor of WeibullMax')
    scale, scale = alpha, beta
    return openturns.dist_bundle3.WeibullMax(scale, shape, gamma)
%}
