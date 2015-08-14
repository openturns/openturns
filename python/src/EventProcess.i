// SWIG file EventProcess.i

%{
#include "EventProcess.hxx"
%}

%include EventProcess.hxx
namespace OT { %extend EventProcess { EventProcess(const EventProcess & other) { return new OT::EventProcess(other); } } }
