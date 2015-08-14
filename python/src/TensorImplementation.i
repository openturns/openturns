// SWIG file TensorImplementation.i

%{
#include "TensorImplementation.hxx"
%}

%include TensorImplementation_doc.i

%rename(TensorImplementation_operator___eq__) OT::operator ==(const TensorImplementation & lhs, const TensorImplementation & rhs);

%include TensorImplementation.hxx
namespace OT{ %extend TensorImplementation { TensorImplementation(const TensorImplementation & other) { return new OT::TensorImplementation(other); } } }
