// SWIG file CalibrationAlgorithmImplementation.i

%{
#include "openturns/CalibrationAlgorithmImplementation.hxx"
%}

%include CalibrationAlgorithmImplementation_doc.i

%include openturns/CalibrationAlgorithmImplementation.hxx
namespace OT { %extend CalibrationAlgorithmImplementation { CalibrationAlgorithmImplementation(const CalibrationAlgorithmImplementation & other) { return new OT::CalibrationAlgorithmImplementation(other); } } }
