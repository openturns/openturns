// SWIG file AdaptiveStrategy.i

%{
#include "openturns/AdaptiveStrategy.hxx"
%}

%include AdaptiveStrategy_doc.i

OTTypedInterfaceObjectHelper(AdaptiveStrategy)

%include openturns/AdaptiveStrategy.hxx
namespace OT{ %extend AdaptiveStrategy { AdaptiveStrategy(const AdaptiveStrategy & other) { return new OT::AdaptiveStrategy(other); } } }
