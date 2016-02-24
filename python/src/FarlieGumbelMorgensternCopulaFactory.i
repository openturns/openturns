// SWIG file FarlieGumbelMorgensternCopulaFactory.i

%{
#include "openturns/FarlieGumbelMorgensternCopulaFactory.hxx"
%}

%include FarlieGumbelMorgensternCopulaFactory_doc.i

%include openturns/FarlieGumbelMorgensternCopulaFactory.hxx
namespace OT { %extend FarlieGumbelMorgensternCopulaFactory { FarlieGumbelMorgensternCopulaFactory(const FarlieGumbelMorgensternCopulaFactory & other) { return new OT::FarlieGumbelMorgensternCopulaFactory(other); } } }
