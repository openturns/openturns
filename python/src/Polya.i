// SWIG file Polya.i

%{
#include "openturns/Polya.hxx"
%}

%include Polya_doc.i

%copyctor OT::Polya;

%include openturns/Polya.hxx

%pythoncode %{
def NegativeBinomial(r, p):
    """
    Retro-compatibility alias for Polya.
    """
    openturns.common.Log.Warn('class NegativeBinomial is deprecated in favor of Polya')
    return openturns.dist_bundle2.Polya(r, p)
%}
