// SWIG file SecondOrderModelImplementation.i

%{
#include "SecondOrderModelImplementation.hxx"
%}

%include SecondOrderModelImplementation_doc.i

%include SecondOrderModelImplementation.hxx
namespace OT{ %extend SecondOrderModelImplementation { SecondOrderModelImplementation(const SecondOrderModelImplementation & other) { return new OT::SecondOrderModelImplementation(other); } } }
