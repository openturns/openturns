// SWIG file CalibrationResult.i

%{
#include "openturns/CalibrationResult.hxx"
%}

%include CalibrationResult_doc.i

%include openturns/CalibrationResult.hxx
namespace OT{ %extend CalibrationResult { CalibrationResult(const CalibrationResult & other) { return new OT::CalibrationResult(other); } } }
