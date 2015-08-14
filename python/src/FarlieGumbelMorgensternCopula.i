// SWIG file FarlieGumbelMorgensternCopula.i

%{
#include "FarlieGumbelMorgensternCopula.hxx"
%}

%include FarlieGumbelMorgensternCopula_doc.i

%include FarlieGumbelMorgensternCopula.hxx
namespace OT { %extend FarlieGumbelMorgensternCopula { FarlieGumbelMorgensternCopula(const FarlieGumbelMorgensternCopula & other) { return new OT::FarlieGumbelMorgensternCopula(other); } } }
