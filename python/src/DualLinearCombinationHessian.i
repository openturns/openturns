// SWIG file DualLinearCombinationHessian.i

%{
#include "openturns/DualLinearCombinationHessian.hxx"
%}

%include DualLinearCombinationHessian_doc.i

%include openturns/DualLinearCombinationHessian.hxx
namespace OT { %extend DualLinearCombinationHessian { DualLinearCombinationHessian(const DualLinearCombinationHessian & other) { return new OT::DualLinearCombinationHessian(other); } } }
