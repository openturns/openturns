// SWIG file BasisSequence.i

%{
#include "openturns/BasisSequence.hxx"
%}

%include BasisSequence_doc.i

OTTypedInterfaceObjectHelper(BasisSequence)

%include openturns/BasisSequence.hxx
namespace OT { %extend BasisSequence { BasisSequence(const BasisSequence & other) { return new OT::BasisSequence(other); } } }
