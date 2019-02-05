// SWIG file UsualRandomVector.i

%{
#include "openturns/UsualRandomVector.hxx"
%}

%include UsualRandomVector_doc.i

%include openturns/UsualRandomVector.hxx
namespace OT { %extend UsualRandomVector { UsualRandomVector(const UsualRandomVector & other) { return new OT::UsualRandomVector(other); } } }
