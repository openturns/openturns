// SWIG file EventRandomVectorImplementation.i

%{
#include "openturns/EventRandomVectorImplementation.hxx"
%}

%include openturns/EventRandomVectorImplementation.hxx
namespace OT { %extend EventRandomVectorImplementation { EventRandomVectorImplementation(const EventRandomVectorImplementation & other) { return new OT::EventRandomVectorImplementation(other); } } }
