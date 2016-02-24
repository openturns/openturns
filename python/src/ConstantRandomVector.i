// SWIG file ConstantRandomVector.i

OTAutodoc(ConstantRandomVector)

%{
#include "openturns/ConstantRandomVector.hxx"
%}

%include openturns/ConstantRandomVector.hxx
namespace OT { %extend ConstantRandomVector { ConstantRandomVector(const ConstantRandomVector & other) { return new OT::ConstantRandomVector(other); } } }
