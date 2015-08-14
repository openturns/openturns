// SWIG file NegativeBinomial.i

%{
#include "NegativeBinomial.hxx"
%}

%include NegativeBinomial_doc.i

%include NegativeBinomial.hxx
namespace OT { %extend NegativeBinomial { NegativeBinomial(const NegativeBinomial & other) { return new OT::NegativeBinomial(other); } } }
