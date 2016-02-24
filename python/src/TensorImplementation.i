// SWIG file TensorImplementation.i

%{
#include "openturns/TensorImplementation.hxx"
%}

%include TensorImplementation_doc.i

%rename(TensorImplementation_operator___eq__) OT::operator ==(const TensorImplementation & lhs, const TensorImplementation & rhs);

%include openturns/TensorImplementation.hxx
namespace OT{ %extend TensorImplementation { TensorImplementation(const TensorImplementation & other) { return new OT::TensorImplementation(other); } } }
