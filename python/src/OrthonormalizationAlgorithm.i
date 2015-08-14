// SWIG file OrthonormalizationAlgorithm.i

%{
#include "OrthonormalizationAlgorithm.hxx"
%}

%include OrthonormalizationAlgorithm_doc.i

OTTypedInterfaceObjectHelper(OrthonormalizationAlgorithm)

%include OrthonormalizationAlgorithm.hxx
namespace OT{ %extend OrthonormalizationAlgorithm { OrthonormalizationAlgorithm(const OrthonormalizationAlgorithm & other) { return new OT::OrthonormalizationAlgorithm(other); } } }
