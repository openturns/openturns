// SWIG file GumbelFactory.i

%{
#include "openturns/GumbelFactory.hxx"
%}

%include GumbelFactory_doc.i

%include openturns/GumbelFactory.hxx
namespace OT { %extend GumbelFactory { GumbelFactory(const GumbelFactory & other) { return new OT::GumbelFactory(other); } } }
