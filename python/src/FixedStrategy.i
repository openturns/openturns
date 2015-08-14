// SWIG file FixedStrategy.i

%{
#include "FixedStrategy.hxx"
%}

%include FixedStrategy_doc.i

%include FixedStrategy.hxx
namespace OT{ %extend FixedStrategy { FixedStrategy(const FixedStrategy & other) { return new OT::FixedStrategy(other); } } }
