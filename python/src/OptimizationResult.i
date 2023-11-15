// SWIG file OptimizationResult.i

%{
#include "openturns/OptimizationResult.hxx"
%}

%include OptimizationResult_doc.i

%ignore OT::OptimizationResult::store;

%copyctor OT::OptimizationResult;

%include openturns/OptimizationResult.hxx
