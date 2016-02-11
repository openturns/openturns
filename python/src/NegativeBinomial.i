// SWIG file NegativeBinomial.i

%{
#include "openturns/NegativeBinomial.hxx"
%}

%include NegativeBinomial_doc.i

%include openturns/NegativeBinomial.hxx
namespace OT { %extend NegativeBinomial { NegativeBinomial(const NegativeBinomial & other) { return new OT::NegativeBinomial(other); } } }
