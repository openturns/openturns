// SWIG file Student.i

%{
#include "openturns/Student.hxx"
%}

%include Student_doc.i

%include openturns/Student.hxx
namespace OT { %extend Student { Student(const Student & other) { return new OT::Student(other); } } }
