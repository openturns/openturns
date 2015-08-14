// SWIG file ConditionalRandomVector.i

%{
#include "ConditionalRandomVector.hxx"
%}

%include ConditionalRandomVector_doc.i

%include ConditionalRandomVector.hxx
namespace OT { %extend ConditionalRandomVector { ConditionalRandomVector(const ConditionalRandomVector & other) { return new OT::ConditionalRandomVector(other); } } }
