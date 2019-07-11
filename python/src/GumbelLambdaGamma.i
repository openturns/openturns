// SWIG file GumbelLambdaGamma.i

%{
#include "openturns/GumbelLambdaGamma.hxx"
%}

%include GumbelLambdaGamma_doc.i

%include openturns/GumbelLambdaGamma.hxx
namespace OT{ %extend GumbelLambdaGamma { GumbelLambdaGamma(const GumbelLambdaGamma & other) { return new OT::GumbelLambdaGamma(other); } } }
