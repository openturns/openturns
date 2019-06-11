// SWIG file WeibullMinMuSigma.i

%{
#include "openturns/WeibullMinMuSigma.hxx"
%}

%include WeibullMinMuSigma_doc.i

%include openturns/WeibullMinMuSigma.hxx
namespace OT{ %extend WeibullMinMuSigma { WeibullMinMuSigma(const WeibullMinMuSigma & other) { return new OT::WeibullMinMuSigma(other); } } }


%pythoncode %{
def WeibullMuSigma(mu, sigma, gamma=0.0):
    openturns.common.Log.Warn('class WeibullMuSigma is deprecated in favor of WeibullMinMuSigma')
    return openturns.dist_bundle3.WeibullMinMuSigma(mu, sigma, gamma)
%}
