// SWIG file TranslationFunction.i

%{
#include "openturns/TranslationFunction.hxx"
%}

%include TranslationFunction_doc.i

%include openturns/TranslationFunction.hxx
namespace OT { %extend TranslationFunction { TranslationFunction(const TranslationFunction & other) { return new OT::TranslationFunction(other); } } }

