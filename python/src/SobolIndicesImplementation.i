// SWIG file SobolIndicesImplementation.i

%{
#include "openturns/SobolIndicesImplementation.hxx"
%}

%include SobolIndicesImplementation_doc.i

%include openturns/SobolIndicesImplementation.hxx
namespace OT{ %extend SobolIndicesImplementation { SobolIndicesImplementation(const SobolIndicesImplementation & other) { return new OT::SobolIndicesImplementation(other); } } }
