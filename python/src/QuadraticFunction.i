// SWIG file QuadraticFunction.i

%{
#include "openturns/QuadraticFunction.hxx"
%}

%include QuadraticFunction_doc.i

%include openturns/QuadraticFunction.hxx
namespace OT { %extend QuadraticFunction { QuadraticFunction(const QuadraticFunction & other) { return new OT::QuadraticFunction(other); } } }

