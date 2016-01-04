// SWIG file ConstantRandomVector.i

OTAutodoc(ConstantRandomVector)

%{
#include "ConstantRandomVector.hxx"
%}

%include ConstantRandomVector.hxx
namespace OT { %extend ConstantRandomVector { ConstantRandomVector(const ConstantRandomVector & other) { return new OT::ConstantRandomVector(other); } } }
