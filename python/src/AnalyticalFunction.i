// SWIG file AnalyticalFunction.i

%{
#include "openturns/AnalyticalFunction.hxx"
%}

%include AnalyticalFunction_doc.i

%ignore OT::AnalyticalFunction::AnalyticalFunction(const String & inputVariableName, const String & formula);

%include openturns/AnalyticalFunction.hxx
namespace OT { %extend AnalyticalFunction { AnalyticalFunction(const AnalyticalFunction & other) { return new OT::AnalyticalFunction(other); } } }
