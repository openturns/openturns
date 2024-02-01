// SWIG file CalibrationAlgorithm.i

%{
#include "openturns/CalibrationAlgorithm.hxx"
%}

%include CalibrationAlgorithm_doc.i

OTTypedInterfaceObjectHelper(CalibrationAlgorithm)

%copyctor OT::CalibrationAlgorithm;

%include openturns/CalibrationAlgorithm.hxx
