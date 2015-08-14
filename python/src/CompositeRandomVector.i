// SWIG file CompositeRandomVector.i

%{
#include "CompositeRandomVector.hxx"
%}

%include CompositeRandomVector.hxx
namespace OT { %extend CompositeRandomVector { CompositeRandomVector(const CompositeRandomVector & other) { return new OT::CompositeRandomVector(other); } } }
