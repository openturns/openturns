// SWIG file FarlieGumbelMorgensternCopula.i

%{
#include "openturns/FarlieGumbelMorgensternCopula.hxx"
%}

%include FarlieGumbelMorgensternCopula_doc.i

%include openturns/FarlieGumbelMorgensternCopula.hxx
namespace OT { %extend FarlieGumbelMorgensternCopula { FarlieGumbelMorgensternCopula(const FarlieGumbelMorgensternCopula & other) { return new OT::FarlieGumbelMorgensternCopula(other); } } }
