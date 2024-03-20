// SWIG file HistoryStrategy.i

%{
#include "openturns/HistoryStrategy.hxx"
%}

%include HistoryStrategy_doc.i

OTTypedInterfaceObjectHelper(HistoryStrategy)

%copyctor OT::HistoryStrategy;

%include openturns/HistoryStrategy.hxx
