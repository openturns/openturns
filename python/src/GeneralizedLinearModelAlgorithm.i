// SWIG file GeneralizedLinearModelAlgorithm.i

%{
#include "GeneralizedLinearModelAlgorithm.hxx"
%}

%include GeneralizedLinearModelAlgorithm_doc.i

%include GeneralizedLinearModelAlgorithm.hxx

namespace OT{ %extend GeneralizedLinearModelAlgorithm { GeneralizedLinearModelAlgorithm(const GeneralizedLinearModelAlgorithm & other) { return new OT::GeneralizedLinearModelAlgorithm(other); } } }
