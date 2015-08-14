// SWIG file MultinomialFactory.i

%{
#include "MultinomialFactory.hxx"
%}

%include MultinomialFactory_doc.i

%include MultinomialFactory.hxx
namespace OT { %extend MultinomialFactory { MultinomialFactory(const MultinomialFactory & other) { return new OT::MultinomialFactory(other); } } }
