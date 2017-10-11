// SWIG file LinearFunction.i

%{
#include "openturns/LinearFunction.hxx"
%}

%include LinearFunction_doc.i

%include openturns/LinearFunction.hxx
namespace OT { %extend LinearFunction { LinearFunction(const LinearFunction & other) { return new OT::LinearFunction(other); } } }

