// SWIG file FiniteDifferenceGradient.i

%{
#include "openturns/FiniteDifferenceGradient.hxx"
%}

%include FiniteDifferenceGradient_doc.i

%include openturns/FiniteDifferenceGradient.hxx
namespace OT { %extend FiniteDifferenceGradient { FiniteDifferenceGradient(const FiniteDifferenceGradient & other) { return new OT::FiniteDifferenceGradient(other); } } }
