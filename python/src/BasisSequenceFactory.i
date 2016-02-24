// SWIG file BasisSequenceFactory.i

%{
#include "openturns/BasisSequenceFactory.hxx"
%}

%include BasisSequenceFactory_doc.i

OTTypedInterfaceObjectHelper(BasisSequenceFactory)

%include openturns/BasisSequenceFactory.hxx
namespace OT { %extend BasisSequenceFactory { BasisSequenceFactory(const BasisSequenceFactory & other) { return new OT::BasisSequenceFactory(other); } } }
