// SWIG file SobolIndices.i

%{
#include "openturns/SobolIndices.hxx"
%}

%include SobolIndices_doc.i

OTTypedInterfaceObjectHelper(SobolIndices)

%include openturns/SobolIndices.hxx
namespace OT{ %extend SobolIndices { SobolIndices(const SobolIndices & other) { return new OT::SobolIndices(other); } } }
