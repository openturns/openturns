// SWIG file ProductFunction.i

%{
#include "openturns/ProductFunction.hxx"
%}

%include ProductFunction_doc.i

%include openturns/ProductFunction.hxx
namespace OT { %extend ProductFunction { ProductFunction(const ProductFunction & other) { return new OT::ProductFunction(other); } } }
