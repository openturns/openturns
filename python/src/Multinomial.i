// SWIG file Multinomial.i

%{
#include "openturns/Multinomial.hxx"
%}

%include Multinomial_doc.i

%include openturns/Multinomial.hxx
namespace OT { %extend Multinomial { Multinomial(const Multinomial & other) { return new OT::Multinomial(other); } } }
