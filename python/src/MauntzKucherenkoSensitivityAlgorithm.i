// SWIG file JansenSensitivityAlgorithm.i

%{
#include "openturns/MauntzKucherenkoSensitivityAlgorithm.hxx"
%}

%include MauntzKucherenkoSensitivityAlgorithm_doc.i

%include openturns/MauntzKucherenkoSensitivityAlgorithm.hxx
namespace OT{ %extend MauntzKucherenkoSensitivityAlgorithm { MauntzKucherenkoSensitivityAlgorithm(const MauntzKucherenkoSensitivityAlgorithm & other) { return new OT::MauntzKucherenkoSensitivityAlgorithm(other); } } }
