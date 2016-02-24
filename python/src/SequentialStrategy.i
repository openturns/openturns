// SWIG file SequentialStrategy.i

%{
#include "openturns/SequentialStrategy.hxx"
%}

%include SequentialStrategy_doc.i

%include openturns/SequentialStrategy.hxx
namespace OT{ %extend SequentialStrategy { SequentialStrategy(const SequentialStrategy & other) { return new OT::SequentialStrategy(other); } } }
