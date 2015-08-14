// SWIG file BasisSequenceFactory.i

%{
#include "BasisSequenceFactory.hxx"
%}

%include BasisSequenceFactory_doc.i

OTTypedInterfaceObjectHelper(BasisSequenceFactory)

%include BasisSequenceFactory.hxx
namespace OT { %extend BasisSequenceFactory { BasisSequenceFactory(const BasisSequenceFactory & other) { return new OT::BasisSequenceFactory(other); } } }
