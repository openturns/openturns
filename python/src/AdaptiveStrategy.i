// SWIG file AdaptiveStrategy.i

%{
#include "AdaptiveStrategy.hxx"
%}

%include AdaptiveStrategy_doc.i

OTTypedInterfaceObjectHelper(AdaptiveStrategy)

%include AdaptiveStrategy.hxx
namespace OT{ %extend AdaptiveStrategy { AdaptiveStrategy(const AdaptiveStrategy & other) { return new OT::AdaptiveStrategy(other); } } }
