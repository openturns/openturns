// SWIG file MartinezSensitivityAlgorithm.i

%{
#include "openturns/MartinezSensitivityAlgorithm.hxx"
%}

%include MartinezSensitivityAlgorithm_doc.i

%include openturns/MartinezSensitivityAlgorithm.hxx
namespace OT{ %extend MartinezSensitivityAlgorithm { MartinezSensitivityAlgorithm(const MartinezSensitivityAlgorithm & other) { return new OT::MartinezSensitivityAlgorithm(other); } } }
