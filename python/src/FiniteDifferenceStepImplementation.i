// SWIG file FiniteDifferenceStepImplementation.i

%{
#include "FiniteDifferenceStepImplementation.hxx"
%}

%include FiniteDifferenceStepImplementation_doc.i

%include FiniteDifferenceStepImplementation.hxx
namespace OT { %extend FiniteDifferenceStepImplementation { FiniteDifferenceStepImplementation(const FiniteDifferenceStepImplementation & other) { return new OT::FiniteDifferenceStepImplementation(other); } } }
