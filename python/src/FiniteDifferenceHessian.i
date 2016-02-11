// SWIG file FiniteDifferenceHessian.i

%{
#include "openturns/FiniteDifferenceHessian.hxx"
%}

%include FiniteDifferenceHessian_doc.i

%include openturns/FiniteDifferenceHessian.hxx
namespace OT { %extend FiniteDifferenceHessian { FiniteDifferenceHessian(const FiniteDifferenceHessian & other) { return new OT::FiniteDifferenceHessian(other); } } }
