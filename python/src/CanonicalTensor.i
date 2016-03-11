// SWIG file CanonicalTensor.i

%{
#include "openturns/CanonicalTensor.hxx"
%}

//%include CanonicalTensor_doc.i

%include openturns/CanonicalTensor.hxx
namespace OT{ %extend CanonicalTensor { CanonicalTensor(const CanonicalTensor & other) { return new OT::CanonicalTensor(other); } } }
