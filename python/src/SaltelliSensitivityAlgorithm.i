// SWIG file SaltelliSensitivityAlgorithm.i

%{
#include "openturns/SaltelliSensitivityAlgorithm.hxx"
%}

%include SaltelliSensitivityAlgorithm_doc.i

%include openturns/SaltelliSensitivityAlgorithm.hxx
%namespace OT{ %extend SaltelliSensitivityAlgorithm { SaltelliSensitivityAlgorithm(const SaltelliSensitivityAlgorithm & other) { return new OT::SaltelliSensitivityAlgorithm(other); } } }
