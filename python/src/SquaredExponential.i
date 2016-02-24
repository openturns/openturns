// SWIG file SquaredExponential.i

%{
#include "openturns/SquaredExponential.hxx"
%}

%include SquaredExponential_doc.i

%include openturns/SquaredExponential.hxx
namespace OT { %extend SquaredExponential { SquaredExponential(const SquaredExponential & other) { return new OT::SquaredExponential(other); } } }
