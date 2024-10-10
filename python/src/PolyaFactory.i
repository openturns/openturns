// SWIG file PolyaFactory.i

%{
#include "openturns/PolyaFactory.hxx"
%}

%include PolyaFactory_doc.i

%copyctor OT::PolyaFactory;

%include openturns/PolyaFactory.hxx

%pythoncode %{
def NegativeBinomialFactory():
    """
    Retro-compatibility alias for PolyaFactory.
    """
    openturns.common.Log.Warn('class NegativeBinomialFactory is deprecated in favor of PolyaFactory')
    return openturns.dist_bundle2.PolyaFactory()
%}
