// SWIG file OptimizationResult.i

%{
#include "openturns/OptimizationResult.hxx"
%}

%include OptimizationResult_doc.i

%include openturns/OptimizationResult.hxx
namespace OT{ %extend OptimizationResult { OptimizationResult(const OptimizationResult & other) { return new OT::OptimizationResult(other); } } }
