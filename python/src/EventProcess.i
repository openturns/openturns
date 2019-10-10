// SWIG file EventProcess.i

%{
#include "openturns/EventProcess.hxx"
%}

%include EventProcess_doc.i

%include openturns/EventProcess.hxx
namespace OT { %extend EventProcess { EventProcess(const EventProcess & other) { return new OT::EventProcess(other); } } }
