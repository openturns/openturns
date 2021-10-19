// SWIG file AdaptiveDirectionalStratification.i

%{
#include "openturns/AdaptiveDirectionalStratification.hxx"
%}

%include AdaptiveDirectionalStratification_doc.i

%include openturns/AdaptiveDirectionalStratification.hxx
namespace OT { %extend AdaptiveDirectionalStratification { AdaptiveDirectionalStratification(const AdaptiveDirectionalStratification & other) { return new OT::AdaptiveDirectionalStratification(other); } } }

