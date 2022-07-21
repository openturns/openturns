// SWIG file CorrelationAnalysis.i

%{
#include "openturns/CorrelationAnalysis.hxx"
%}

%include CorrelationAnalysis_doc.i

%include openturns/CorrelationAnalysis.hxx
namespace OT{ %extend CorrelationAnalysis { CorrelationAnalysis(const CorrelationAnalysis & other) { return new OT::CorrelationAnalysis(other); } } }
