// SWIG file ARMAFactory.i

%{
#include "openturns/ARMAFactory.hxx"
%}

%include ARMAFactory_doc.i

OTTypedInterfaceObjectHelper(ARMAFactory)

%include openturns/ARMAFactory.hxx
namespace OT { %extend ARMAFactory { ARMAFactory(const ARMAFactory & other) { return new OT::ARMAFactory(other); } } }

