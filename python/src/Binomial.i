// SWIG file Binomial.i

%{
#include "openturns/Binomial.hxx"
%}

%include Binomial_doc.i

%include openturns/Binomial.hxx
namespace OT { %extend Binomial { Binomial(const Binomial & other) { return new OT::Binomial(other); } } }
