// SWIG file NonCentralStudent.i

%{
#include "NonCentralStudent.hxx"
%}

%include NonCentralStudent_doc.i

%include NonCentralStudent.hxx
namespace OT { %extend NonCentralStudent { NonCentralStudent(const NonCentralStudent & other) { return new OT::NonCentralStudent(other); } } }
