// SWIG file ConditionalRandomVector.i

%{
#include "openturns/ConditionalRandomVector.hxx"
%}

%include ConditionalRandomVector_doc.i

%include openturns/ConditionalRandomVector.hxx
namespace OT { %extend ConditionalRandomVector { ConditionalRandomVector(const ConditionalRandomVector & other) { return new OT::ConditionalRandomVector(other); } } }
