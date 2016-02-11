// SWIG file ComplexTensorImplementation.i

%{
#include "openturns/ComplexTensorImplementation.hxx"
%}

%rename(ComplexTensorImplementation_operator___eq__) OT::operator ==(const ComplexTensorImplementation & lhs, const ComplexTensorImplementation & rhs);

%include openturns/ComplexTensorImplementation.hxx
namespace OT{ %extend ComplexTensorImplementation { ComplexTensorImplementation(const ComplexTensorImplementation & other) { return new OT::ComplexTensorImplementation(other); } } }
