// SWIG file NegativeBinomialFactory.i

%{
#include "NegativeBinomialFactory.hxx"
%}

%include NegativeBinomialFactory_doc.i

%include NegativeBinomialFactory.hxx
namespace OT { %extend NegativeBinomialFactory { NegativeBinomialFactory(const NegativeBinomialFactory & other) { return new OT::NegativeBinomialFactory(other); } } }
