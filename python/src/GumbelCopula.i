// SWIG file GumbelCopula.i

%{
#include "GumbelCopula.hxx"
%}

%include GumbelCopula_doc.i

%include GumbelCopula.hxx
namespace OT { %extend GumbelCopula { GumbelCopula(const GumbelCopula & other) { return new OT::GumbelCopula(other); } } }
