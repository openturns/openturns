// SWIG file FiniteDifferenceHessian.i

%{
#include "FiniteDifferenceHessian.hxx"
%}

%include FiniteDifferenceHessian_doc.i

%include FiniteDifferenceHessian.hxx
namespace OT { %extend FiniteDifferenceHessian { FiniteDifferenceHessian(const FiniteDifferenceHessian & other) { return new OT::FiniteDifferenceHessian(other); } } }
