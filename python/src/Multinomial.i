// SWIG file Multinomial.i

%{
#include "Multinomial.hxx"
%}

%include Multinomial_doc.i

%include Multinomial.hxx
namespace OT { %extend Multinomial { Multinomial(const Multinomial & other) { return new OT::Multinomial(other); } } }
