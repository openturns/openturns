// SWIG file ConstantRandomVector.i

%{
#include "openturns/ConstantRandomVector.hxx"
%}

%include ConstantRandomVector_doc.i

%include openturns/ConstantRandomVector.hxx
namespace OT { %extend ConstantRandomVector { ConstantRandomVector(const ConstantRandomVector & other) { return new OT::ConstantRandomVector(other); } } }
