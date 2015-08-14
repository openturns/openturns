// SWIG file SequentialStrategy.i

%{
#include "SequentialStrategy.hxx"
%}

%include SequentialStrategy_doc.i

%include SequentialStrategy.hxx
namespace OT{ %extend SequentialStrategy { SequentialStrategy(const SequentialStrategy & other) { return new OT::SequentialStrategy(other); } } }
