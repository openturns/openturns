// SWIG file CompositeRandomVector.i

%{
#include "openturns/CompositeRandomVector.hxx"
%}

%include CompositeRandomVector_doc.i

%include openturns/CompositeRandomVector.hxx
namespace OT { %extend CompositeRandomVector { CompositeRandomVector(const CompositeRandomVector & other) { return new OT::CompositeRandomVector(other); } } }
