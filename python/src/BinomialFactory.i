// SWIG file BinomialFactory.i

%{
#include "openturns/BinomialFactory.hxx"
%}

%include BinomialFactory_doc.i

%include openturns/BinomialFactory.hxx
namespace OT { %extend BinomialFactory { BinomialFactory(const BinomialFactory & other) { return new OT::BinomialFactory(other); } } }
