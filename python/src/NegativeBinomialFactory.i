// SWIG file NegativeBinomialFactory.i

%{
#include "openturns/NegativeBinomialFactory.hxx"
%}

%include NegativeBinomialFactory_doc.i

%include openturns/NegativeBinomialFactory.hxx
namespace OT { %extend NegativeBinomialFactory { NegativeBinomialFactory(const NegativeBinomialFactory & other) { return new OT::NegativeBinomialFactory(other); } } }
