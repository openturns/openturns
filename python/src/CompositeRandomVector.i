// SWIG file CompositeRandomVector.i

OTAutodoc(CompositeRandomVector)

%{
#include "openturns/CompositeRandomVector.hxx"
%}

%include openturns/CompositeRandomVector.hxx
namespace OT { %extend CompositeRandomVector { CompositeRandomVector(const CompositeRandomVector & other) { return new OT::CompositeRandomVector(other); } } }
