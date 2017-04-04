// SWIG file ProductHessian.i

OTAutodoc(ProductHessian)

%{
#include "openturns/ProductHessian.hxx"
%}

%include openturns/ProductHessian.hxx
namespace OT { %extend ProductHessian { ProductHessian(const ProductHessian & other) { return new OT::ProductHessian(other); } } }
