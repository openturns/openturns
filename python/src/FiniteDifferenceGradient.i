// SWIG file FiniteDifferenceGradient.i

%{
#include "FiniteDifferenceGradient.hxx"
%}

%include FiniteDifferenceGradient_doc.i

%include FiniteDifferenceGradient.hxx
namespace OT { %extend FiniteDifferenceGradient { FiniteDifferenceGradient(const FiniteDifferenceGradient & other) { return new OT::FiniteDifferenceGradient(other); } } }
