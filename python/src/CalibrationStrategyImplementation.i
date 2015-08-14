// SWIG file CalibrationStrategyImplementation.i

%{
#include "CalibrationStrategyImplementation.hxx"
%}

%include CalibrationStrategyImplementation_doc.i

%include CalibrationStrategyImplementation.hxx
namespace OT { %extend CalibrationStrategyImplementation { CalibrationStrategyImplementation(const CalibrationStrategyImplementation & other) { return new OT::CalibrationStrategyImplementation(other); } } }
