// SWIG file OrthonormalizationAlgorithmImplementation.i

%{
#include "OrthonormalizationAlgorithmImplementation.hxx"
%}

%include OrthonormalizationAlgorithmImplementation_doc.i

%include OrthonormalizationAlgorithmImplementation.hxx
namespace OT{ %extend OrthonormalizationAlgorithmImplementation { OrthonormalizationAlgorithmImplementation(const OrthonormalizationAlgorithmImplementation & other) { return new OT::OrthonormalizationAlgorithmImplementation(other); } } }
