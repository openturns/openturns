// SWIG file WeibullMin.i

%{
#include "openturns/WeibullMin.hxx"
%}

%include WeibullMin_doc.i

%include openturns/WeibullMin.hxx
namespace OT { %extend WeibullMin { WeibullMin(const WeibullMin & other) { return new OT::WeibullMin(other); } } }

%pythoncode %{
def Weibull(*args):
    openturns.common.Log.Warn('class Weibull is deprecated in favor of WeibullMin')
    return openturns.dist_bundle3.WeibullMin(*args)
%}
