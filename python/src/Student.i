// SWIG file Student.i

%{
#include "Student.hxx"
%}

%include Student_doc.i

%include Student.hxx
namespace OT { %extend Student { Student(const Student & other) { return new OT::Student(other); } } }
