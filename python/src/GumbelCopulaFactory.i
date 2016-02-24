// SWIG file GumbelCopulaFactory.i

%{
#include "openturns/GumbelCopulaFactory.hxx"
%}

%include GumbelCopulaFactory_doc.i

%include openturns/GumbelCopulaFactory.hxx
namespace OT { %extend GumbelCopulaFactory { GumbelCopulaFactory(const GumbelCopulaFactory & other) { return new OT::GumbelCopulaFactory(other); } } }
