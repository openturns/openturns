// SWIG file EventProcess.i

OTAutodoc(EventProcess)

%{
#include "openturns/EventProcess.hxx"
%}

%include openturns/EventProcess.hxx
namespace OT { %extend EventProcess { EventProcess(const EventProcess & other) { return new OT::EventProcess(other); } } }
