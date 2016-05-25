// SWIG file SobolIndicesAlgorithm.i

%{
#include "openturns/SobolIndicesAlgorithm.hxx"
%}

%include SobolIndicesAlgorithm_doc.i

OTTypedInterfaceObjectHelper(SobolIndicesAlgorithm)

%include openturns/SobolIndicesAlgorithm.hxx
namespace OT{ %extend SobolIndicesAlgorithm { SobolIndicesAlgorithm(const SobolIndicesAlgorithm & other) { return new OT::SobolIndicesAlgorithm(other); } } }
