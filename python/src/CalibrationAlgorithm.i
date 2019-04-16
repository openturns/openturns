// SWIG file CalibrationAlgorithm.i

%{
#include "openturns/CalibrationAlgorithm.hxx"
%}

%include CalibrationAlgorithm_doc.i

OTTypedInterfaceObjectHelper(CalibrationAlgorithm)

%include openturns/CalibrationAlgorithm.hxx
namespace OT { %extend CalibrationAlgorithm { CalibrationAlgorithm(const CalibrationAlgorithm & other) { return new OT::CalibrationAlgorithm(other); } } }
