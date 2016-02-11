// SWIG file SecondOrderModelImplementation.i

%{
#include "openturns/SecondOrderModelImplementation.hxx"
%}

%include SecondOrderModelImplementation_doc.i

%include openturns/SecondOrderModelImplementation.hxx
namespace OT{ %extend SecondOrderModelImplementation { SecondOrderModelImplementation(const SecondOrderModelImplementation & other) { return new OT::SecondOrderModelImplementation(other); } } }
