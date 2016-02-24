// SWIG file GeneralizedLinearModelAlgorithm.i

%{
#include "openturns/GeneralizedLinearModelAlgorithm.hxx"
%}

%include GeneralizedLinearModelAlgorithm_doc.i

%include openturns/GeneralizedLinearModelAlgorithm.hxx

namespace OT{ %extend GeneralizedLinearModelAlgorithm { GeneralizedLinearModelAlgorithm(const GeneralizedLinearModelAlgorithm & other) { return new OT::GeneralizedLinearModelAlgorithm(other); } } }
