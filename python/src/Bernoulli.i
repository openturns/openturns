// SWIG file Bernoulli.i

%{
#include "openturns/Bernoulli.hxx"
%}

%include Bernoulli_doc.i

%include openturns/Bernoulli.hxx
namespace OT { %extend Bernoulli { Bernoulli(const Bernoulli & other) { return new OT::Bernoulli(other); } } }
