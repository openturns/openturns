// SWIG file RootStrategy.i

%{
#include "openturns/RootStrategy.hxx"
%}

%include RootStrategy_doc.i

OTTypedInterfaceObjectHelper(RootStrategy)

%include openturns/RootStrategy.hxx
namespace OT{ %extend RootStrategy { RootStrategy(const RootStrategy & other) { return new OT::RootStrategy(other); } } }
