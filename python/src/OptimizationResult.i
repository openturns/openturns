// SWIG file OptimizationResult.i

%{
#include "OptimizationResult.hxx"
%}

%include OptimizationResult_doc.i

%include OptimizationResult.hxx
namespace OT{ %extend OptimizationResult { OptimizationResult(const OptimizationResult & other) { return new OT::OptimizationResult(other); } } }
