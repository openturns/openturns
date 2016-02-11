// SWIG file GumbelCopula.i

%{
#include "openturns/GumbelCopula.hxx"
%}

%include GumbelCopula_doc.i

%include openturns/GumbelCopula.hxx
namespace OT { %extend GumbelCopula { GumbelCopula(const GumbelCopula & other) { return new OT::GumbelCopula(other); } } }
