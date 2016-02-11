// SWIG file MultinomialFactory.i

%{
#include "openturns/MultinomialFactory.hxx"
%}

%include MultinomialFactory_doc.i

%include openturns/MultinomialFactory.hxx
namespace OT { %extend MultinomialFactory { MultinomialFactory(const MultinomialFactory & other) { return new OT::MultinomialFactory(other); } } }
