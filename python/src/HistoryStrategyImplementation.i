// SWIG file HistoryStrategyImplementation.i

%{
#include "openturns/HistoryStrategyImplementation.hxx"
%}

%include HistoryStrategyImplementation_doc.i

%include openturns/HistoryStrategyImplementation.hxx
namespace OT{ %extend HistoryStrategyImplementation { HistoryStrategyImplementation(const HistoryStrategyImplementation & other) { return new OT::HistoryStrategyImplementation(other); } } }
