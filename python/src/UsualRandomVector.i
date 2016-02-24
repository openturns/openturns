// SWIG file UsualRandomVector.i

OTAutodoc(UsualRandomVector)

%{
#include "openturns/UsualRandomVector.hxx"
%}

%include openturns/UsualRandomVector.hxx
namespace OT { %extend UsualRandomVector { UsualRandomVector(const UsualRandomVector & other) { return new OT::UsualRandomVector(other); } } }
