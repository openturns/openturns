// SWIG file JansenSensitivityAlgorithm.i

%{
#include "openturns/JansenSensitivityAlgorithm.hxx"
%}

%include JansenSensitivityAlgorithm_doc.i

%include openturns/JansenSensitivityAlgorithm.hxx
namespace OT{ %extend JansenSensitivityAlgorithm { JansenSensitivityAlgorithm(const JansenSensitivityAlgorithm & other) { return new OT::JansenSensitivityAlgorithm(other); } } }
