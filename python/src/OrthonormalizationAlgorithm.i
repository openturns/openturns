// SWIG file OrthonormalizationAlgorithm.i

%{
#include "openturns/OrthonormalizationAlgorithm.hxx"
%}

%include OrthonormalizationAlgorithm_doc.i

OTTypedInterfaceObjectHelper(OrthonormalizationAlgorithm)

%include openturns/OrthonormalizationAlgorithm.hxx
namespace OT{ %extend OrthonormalizationAlgorithm { OrthonormalizationAlgorithm(const OrthonormalizationAlgorithm & other) { return new OT::OrthonormalizationAlgorithm(other); } } }
