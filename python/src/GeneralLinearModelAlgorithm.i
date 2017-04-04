// SWIG file GeneralLinearModelAlgorithm.i

%{
#include "openturns/GeneralLinearModelAlgorithm.hxx"
%}

%include GeneralLinearModelAlgorithm_doc.i

%include openturns/GeneralLinearModelAlgorithm.hxx

namespace OT{ %extend GeneralLinearModelAlgorithm { GeneralLinearModelAlgorithm(const GeneralLinearModelAlgorithm & other) { return new OT::GeneralLinearModelAlgorithm(other); } } }

