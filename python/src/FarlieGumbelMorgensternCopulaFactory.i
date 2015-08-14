// SWIG file FarlieGumbelMorgensternCopulaFactory.i

%{
#include "FarlieGumbelMorgensternCopulaFactory.hxx"
%}

%include FarlieGumbelMorgensternCopulaFactory_doc.i

%include FarlieGumbelMorgensternCopulaFactory.hxx
namespace OT { %extend FarlieGumbelMorgensternCopulaFactory { FarlieGumbelMorgensternCopulaFactory(const FarlieGumbelMorgensternCopulaFactory & other) { return new OT::FarlieGumbelMorgensternCopulaFactory(other); } } }
