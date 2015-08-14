// SWIG file Bernoulli.i

%{
#include "Bernoulli.hxx"
%}

%include Bernoulli_doc.i

%include Bernoulli.hxx
namespace OT { %extend Bernoulli { Bernoulli(const Bernoulli & other) { return new OT::Bernoulli(other); } } }
