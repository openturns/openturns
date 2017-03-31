// SWIG file IndicatorFunction.i

%{
#include "openturns/IndicatorFunction.hxx"
%}

%include IndicatorFunction_doc.i

%include openturns/IndicatorFunction.hxx
namespace OT { %extend IndicatorFunction { IndicatorFunction(const IndicatorFunction & other) { return new OT::IndicatorFunction(other); } } }
