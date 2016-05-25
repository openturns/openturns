// SWIG file SobolIndicesAlgorithmImplementation.i

%{
#include "openturns/SobolIndicesAlgorithmImplementation.hxx"
%}

%include SobolIndicesAlgorithmImplementation_doc.i

%include openturns/SobolIndicesAlgorithmImplementation.hxx
namespace OT{ %extend SobolIndicesAlgorithmImplementation { SobolIndicesAlgorithmImplementation(const SobolIndicesAlgorithmImplementation & other) { return new OT::SobolIndicesAlgorithmImplementation(other); } } }
