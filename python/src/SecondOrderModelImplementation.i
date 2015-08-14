// SWIG file SecondOrderModelImplementation.i

%{
#include "SecondOrderModelImplementation.hxx"
%}

%include SecondOrderModelImplementation.hxx
namespace OT{ %extend SecondOrderModelImplementation { SecondOrderModelImplementation(const SecondOrderModelImplementation & other) { return new OT::SecondOrderModelImplementation(other); } } }
