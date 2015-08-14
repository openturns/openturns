// SWIG file GumbelCopulaFactory.i

%{
#include "GumbelCopulaFactory.hxx"
%}

%include GumbelCopulaFactory_doc.i

%include GumbelCopulaFactory.hxx
namespace OT { %extend GumbelCopulaFactory { GumbelCopulaFactory(const GumbelCopulaFactory & other) { return new OT::GumbelCopulaFactory(other); } } }
