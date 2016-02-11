// SWIG file BasisSequenceImplementation.i

%{
#include "openturns/BasisSequenceImplementation.hxx"
%}

%include BasisSequenceImplementation_doc.i

%include openturns/BasisSequenceImplementation.hxx
namespace OT { %extend BasisSequenceImplementation { BasisSequenceImplementation(const BasisSequenceImplementation & other) { return new OT::BasisSequenceImplementation(other); } } }
