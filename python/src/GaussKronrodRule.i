// SWIG file GaussKronrodRule.i

%{
#include "openturns/GaussKronrodRule.hxx"
%}

%include GaussKronrodRule_doc.i

%include openturns/GaussKronrodRule.hxx
namespace OT { %extend GaussKronrodRule { GaussKronrodRule(const GaussKronrodRule & other) { return new OT::GaussKronrodRule(other); } } }
