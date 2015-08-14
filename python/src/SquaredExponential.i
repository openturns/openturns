// SWIG file SquaredExponential.i

%{
#include "SquaredExponential.hxx"
%}

%include SquaredExponential_doc.i

%include SquaredExponential.hxx
namespace OT { %extend SquaredExponential { SquaredExponential(const SquaredExponential & other) { return new OT::SquaredExponential(other); } } }
