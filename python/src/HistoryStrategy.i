// SWIG file HistoryStrategy.i

%{
#include "HistoryStrategy.hxx"
%}

OTTypedInterfaceObjectHelper(HistoryStrategy)

%include HistoryStrategy.hxx
namespace OT{ %extend HistoryStrategy { HistoryStrategy(const HistoryStrategy & other) { return new OT::HistoryStrategy(other); } } }
