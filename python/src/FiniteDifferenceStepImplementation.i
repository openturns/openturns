// SWIG file FiniteDifferenceStepImplementation.i

%{
#include "openturns/FiniteDifferenceStepImplementation.hxx"
%}

%include FiniteDifferenceStepImplementation_doc.i

%include openturns/FiniteDifferenceStepImplementation.hxx
namespace OT { %extend FiniteDifferenceStepImplementation { FiniteDifferenceStepImplementation(const FiniteDifferenceStepImplementation & other) { return new OT::FiniteDifferenceStepImplementation(other); } } }
