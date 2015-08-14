// SWIG file ARMAFactory.i

%{
#include "ARMAFactory.hxx"
%}

%include ARMAFactory_doc.i

OTTypedInterfaceObjectHelper(ARMAFactory)

%include ARMAFactory.hxx
namespace OT { %extend ARMAFactory { ARMAFactory(const ARMAFactory & other) { return new OT::ARMAFactory(other); } } }

