// SWIG file Binomial.i

%{
#include "Binomial.hxx"
%}

%include Binomial_doc.i

%include Binomial.hxx
namespace OT { %extend Binomial { Binomial(const Binomial & other) { return new OT::Binomial(other); } } }
