// SWIG file BinomialFactory.i

%{
#include "BinomialFactory.hxx"
%}

%include BinomialFactory_doc.i

%include BinomialFactory.hxx
namespace OT { %extend BinomialFactory { BinomialFactory(const BinomialFactory & other) { return new OT::BinomialFactory(other); } } }
