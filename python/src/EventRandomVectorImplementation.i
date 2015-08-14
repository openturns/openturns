// SWIG file EventRandomVectorImplementation.i

%{
#include "EventRandomVectorImplementation.hxx"
%}

%include EventRandomVectorImplementation.hxx
namespace OT { %extend EventRandomVectorImplementation { EventRandomVectorImplementation(const EventRandomVectorImplementation & other) { return new OT::EventRandomVectorImplementation(other); } } }
