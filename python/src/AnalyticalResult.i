// SWIG file AnalyticalResult.i

%{
#include "openturns/AnalyticalResult.hxx"
%}

%include AnalyticalResult_doc.i

%include openturns/AnalyticalResult.hxx
namespace OT{ %extend AnalyticalResult { AnalyticalResult(const AnalyticalResult & other) { return new OT::AnalyticalResult(other); } } }
