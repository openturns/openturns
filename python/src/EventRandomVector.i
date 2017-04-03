// SWIG file EventRandomVector.i

%{
#include "openturns/EventRandomVector.hxx"
%}

%include openturns/EventRandomVector.hxx
namespace OT { %extend EventRandomVector { EventRandomVector(const EventRandomVector & other) { return new OT::EventRandomVector(other); } } }
