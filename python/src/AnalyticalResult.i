// SWIG file AnalyticalResult.i

%{
#include "AnalyticalResult.hxx"
%}

%include AnalyticalResult_doc.i

%include AnalyticalResult.hxx
namespace OT{ %extend AnalyticalResult { AnalyticalResult(const AnalyticalResult & other) { return new OT::AnalyticalResult(other); } } }
