// SWIG file GaussKronrodRule.i

%{
#include "GaussKronrodRule.hxx"
%}

%include GaussKronrodRule_doc.i

%include GaussKronrodRule.hxx
namespace OT { %extend GaussKronrodRule { GaussKronrodRule(const GaussKronrodRule & other) { return new OT::GaussKronrodRule(other); } } }
