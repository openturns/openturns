// SWIG file CalibrationStrategyImplementation.i

%{
#include "openturns/CalibrationStrategyImplementation.hxx"
%}

%include CalibrationStrategyImplementation_doc.i

%include openturns/CalibrationStrategyImplementation.hxx
namespace OT { %extend CalibrationStrategyImplementation { CalibrationStrategyImplementation(const CalibrationStrategyImplementation & other) { return new OT::CalibrationStrategyImplementation(other); } } }
