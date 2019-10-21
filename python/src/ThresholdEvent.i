// SWIG file ThresholdEvent.i

%{
#include "openturns/ThresholdEvent.hxx"
%}

%include ThresholdEvent_doc.i

%include openturns/ThresholdEvent.hxx
namespace OT { %extend ThresholdEvent { ThresholdEvent(const ThresholdEvent & other) { return new OT::ThresholdEvent(other); } } }
