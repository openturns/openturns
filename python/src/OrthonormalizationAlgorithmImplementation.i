// SWIG file OrthonormalizationAlgorithmImplementation.i

%{
#include "openturns/OrthonormalizationAlgorithmImplementation.hxx"
%}

%include OrthonormalizationAlgorithmImplementation_doc.i

%include openturns/OrthonormalizationAlgorithmImplementation.hxx
namespace OT{ %extend OrthonormalizationAlgorithmImplementation { OrthonormalizationAlgorithmImplementation(const OrthonormalizationAlgorithmImplementation & other) { return new OT::OrthonormalizationAlgorithmImplementation(other); } } }
