// SWIG file BasisSequenceImplementation.i

%{
#include "BasisSequenceImplementation.hxx"
%}

%include BasisSequenceImplementation_doc.i

%include BasisSequenceImplementation.hxx
namespace OT { %extend BasisSequenceImplementation { BasisSequenceImplementation(const BasisSequenceImplementation & other) { return new OT::BasisSequenceImplementation(other); } } }
