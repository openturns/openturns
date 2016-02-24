// SWIG file HistoryStrategy.i

%{
#include "openturns/HistoryStrategy.hxx"
%}

%include HistoryStrategy_doc.i

OTTypedInterfaceObjectHelper(HistoryStrategy)

%include openturns/HistoryStrategy.hxx
namespace OT{ %extend HistoryStrategy { HistoryStrategy(const HistoryStrategy & other) { return new OT::HistoryStrategy(other); } } }
