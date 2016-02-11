// SWIG file FixedStrategy.i

%{
#include "openturns/FixedStrategy.hxx"
%}

%include FixedStrategy_doc.i

%include openturns/FixedStrategy.hxx
namespace OT{ %extend FixedStrategy { FixedStrategy(const FixedStrategy & other) { return new OT::FixedStrategy(other); } } }
