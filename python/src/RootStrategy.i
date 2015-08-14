// SWIG file RootStrategy.i

%{
#include "RootStrategy.hxx"
%}

%include RootStrategy_doc.i

OTTypedInterfaceObjectHelper(RootStrategy)

%include RootStrategy.hxx
namespace OT{ %extend RootStrategy { RootStrategy(const RootStrategy & other) { return new OT::RootStrategy(other); } } }
