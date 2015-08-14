// SWIG file UsualRandomVector.i

%{
#include "UsualRandomVector.hxx"
%}

%include UsualRandomVector.hxx
namespace OT { %extend UsualRandomVector { UsualRandomVector(const UsualRandomVector & other) { return new OT::UsualRandomVector(other); } } }
