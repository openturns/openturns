// SWIG file CanonicalTensorGradient.i

%{
#include "openturns/CanonicalTensorGradient.hxx"
%}

%include CanonicalTensorGradient_doc.i

%include openturns/CanonicalTensorGradient.hxx
namespace OT { %extend CanonicalTensorGradient { CanonicalTensorGradient(const CanonicalTensorGradient & other) { return new OT::CanonicalTensorGradient(other); } } }
