// SWIG file NonCentralStudent.i

%{
#include "openturns/NonCentralStudent.hxx"
%}

%include NonCentralStudent_doc.i

%include openturns/NonCentralStudent.hxx
namespace OT { %extend NonCentralStudent { NonCentralStudent(const NonCentralStudent & other) { return new OT::NonCentralStudent(other); } } }
